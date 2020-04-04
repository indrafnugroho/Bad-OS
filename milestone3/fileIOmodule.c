#include "fileIOmodule.h"

void readFile(char *buffer, char *path, int *result, char parentIndex) {
	char files[1024];
	char tempBuffer[512];
	char tempBuffer2[512];
	int isFound = 0;
	int isNameMatch, k, s, idxName;
	int h, l;
	int j = 0;
	int sectorSize = 512;

	readSector(&files, 257);
	readSector(&tempBuffer, 258);
	for (k = sectorSize; k < sectorSize * 2; k++) {
		files[k] = tempBuffer[k-512];
	}
	
	k = 0;
	while (!isFound && k < sectorSize * 2) {
        // Search for parent idx w/ matching path name
        for (k = 0; k < sectorSize * 2; k += 16) {
			if (files[k] == parentIndex) {
            	if (files[k + 1] != 0xFF && files[k + 2] != 0x0) {
                	idxName = k + 2;
                
                	//matching name
                	isNameMatch = 1;
	                for (h = 0; h < 14; h++) {
    	                if (path[h] != files[h + idxName]) {
        	                isNameMatch = 0;
							break;
                	    } else if (files[h + idxName] == '\0' && path[h] == '\0') {
                    		break;
                    	}
                	}

                	if (isNameMatch) {
                    	isFound = 1;
                    	s = files[k + 1];
						break;
                	}
            	}
        	}
		}
    }
	
	if (!isFound) {
		*result = -1;
		// printString("ga nemu gaes\r\n");
	}
	else {
		readSector(&tempBuffer,259);
		while ((j < 16) && (tempBuffer[j + s * 16] != '\0')) {
			readSector(&tempBuffer2, tempBuffer[j + s * 16]);
			for(l = 0; l < sectorSize; ++l) {
				buffer[sectorSize * j + l] = tempBuffer2[l];
			} ++j;
		} *result = 1; //ini kayanya fix
		// printString("ini bisa gaes\r\n");
	}
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
	char map[512];
	char files[1024];
	int i, countSector;
	char idxParent = parentIndex;
	int entryIndex;
	int isFound, isNameAlreadyExists, j, k, h, m, isNameTrue, n;
	int emptySector;
	char bufferSector[512];

	// Baca sektor di map cukup apa nggak
	readSector(map, 0x100);
	for (i = 0, countSector = 0; i < 256 && countSector < *sectors; i++) {
		if (map[i] == 0x00) {
			countSector++;
		}
	}

	// Apabila jumlah sektor di map tidak cukup
	if (countSector < *sectors) {
		*sectors = 0;
		return;
	}

	// Mencari entry yang kosong pada files
	readSector(files, 0x101);
	readSector(files, 0x102);
	for (entryIndex = 0; entryIndex < 1024; entryIndex += 16) {
		if (files[entryIndex] == '\0') {
			break;
		}
	}

	// Apabila tidak ada entry yang kosong
	if (entryIndex == 1024) {
		*sectors = -3;
	}


	// Sama kayak di readfile, pake while sampe gaada / lagi (file paling ujung)
	// Misal abc/def/g, looping sampe dapet g.
	// def simpen di variabel, misalnya x

	isFound = 0;
	isNameAlreadyExists = 1;
	i = 0;
	j = 0;
	while (!isFound) {
		j = i;
		// isNameTrue = 0;
		while (path[i] != '/' && path[i] != '\0') {
			i++;
		}

		//finding nemo
		k;
		if (path[i] == '\0') {
			// Kalo udah di file terakhir (paling ujung gaada / lagi), cek ada yang namanya sama gak
			isFound = 1;

			//search for parent idx with matching path name
			for (k=0; k < 1024; k+=16) {
				if (files[k] == idxParent) {
					m = k+2;
					//matching name
					h;
					for (h=0; h < i-j-1; h++) {
						if (path[j+h] != files[m+h]) {
							break;
						}
					} 
					if (h == i-j-1) {
						isNameAlreadyExists = 0;
						break;
					}
				}
			}
		} else {
			
			//search for parent idx with matching path name
			for (k=0; k < 1024; k+=16) {
				if (files[k] == idxParent) {
					m = k+2;
					//matching name
					h;
					for (h=0; h < i-j-1; h++) {
						if (path[j+h] != files[m+h]) {
							break;
						}
					} if (h == i-j-1) {
						isNameTrue = 1;
						idxParent = k; //in hexa gengs
						break;
					}
				}
			}

			if (k==1024) break; // break while terluar
			i++;
		}
	}

	if (!isFound) {
		*sectors = -1;
		return;
	} else if (!isNameAlreadyExists) {
		*sectors = 2;
		return;
	} else {
		readSector(bufferSector, 0x103);
		emptySector = 0;

		while (emptySector < 16 && bufferSector[(idxParent + 1) * 16 + emptySector] != '\0') {
			char buffTemp[512];
			readSector(&buffTemp, bufferSector[(idxParent + 1) * 16 + emptySector]);
			for (i = 0; i < 512; i++) {
				buffer[i + 512 * emptySector] = buffTemp[i];
			}
			emptySector++;
		}

		*sectors = 1;
	}

	// Tulis indeks parent diisi ama x
}

void delFile(char entry) {
	char mapBuffer[512], folderAndFiles[1024], sectBuffer[512];
	int i;
	
	//read sector 257 and 258
	interrupt(0x21, 0x02, folderAndFiles, 257, 0);
	interrupt(0x21, 0x02, folderAndFiles + 512, 258, 0);
	
	//delete file entry
	folderAndFiles[entry * 16] = 0x0;
	folderAndFiles[entry * 16 + 1] = '\0';
		
	//deletion in map and sector
	interrupt(0x21, 0x02, &mapBuffer, 256, 0);
	interrupt(0x21, 0x02, &sectBuffer, 259, 0);
	i = 0;
	while (sectBuffer[entry * 16 + i] != '\0' && i < 16) {
		//make it empty
		mapBuffer[sectBuffer[entry * 16 + i]] = 0x0;
		sectBuffer[entry * 16 + i] = 0x0;
		i++;
	}
	//rewrite buffer to sectors
	interrupt(0x21, 0x03, &folderAndFiles, 257, 0);
	interrupt(0x21, 0x03, folderAndFiles + 512, 258, 0);
	interrupt(0x21, 0x03, &sectBuffer, 259, 0);
	interrupt(0x21, 0x03, &mapBuffer, 256, 0);
}

void readSector(char *buffer, int sector) {
	interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
	interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}