/* Ini deklarasi fungsi */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string); //done
void readString(char *string); //done
void readSector(char *buffer, int sector); //done
void writeSector(char *buffer, int sector); //done
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0 (done)
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void executeProgram(char *filename, int segment, int *success);
void printLogo();

//Fungsi Matematika
int mod(int x, int y); //done
int div(int a,int b); //done

//Main Function
int main() {
	char buffer[512 * 16];
	int suc;
	printString("JANCOK");
	printLogo();
	printString("sini");
	makeInterrupt21();
	printString("sini2");
	interrupt(0x21, 0x4, buffer, "key.txt", &suc);
	printString("sini3");
	if (suc) {
		interrupt(0x21,0x0, "Key : ", 0, 0);
	 	interrupt(0x21,0x0, buffer, 0, 0);
	}
	else {
		interrupt(0x21, 0x6, "milestone1", 0x2000, &suc);
	}
	while (1);
}


void handleInterrupt21 (int AX, int BX, int CX, int DX) {
	char AL, AH;
	AL = (char) (AX);
	AH = (char) (AX >> 8);
	switch (AL) {
		case 0x00:
			printString(BX);
			break;
		case 0x01:
			readString(BX);
			break;
		case 0x02:
			readSector(BX, CX);
			break;
		case 0x03:
			writeSector(BX, CX);
			break;
		case 0x04:
			readFile(BX, CX, DX, AH);
			break;
		case 0x05:
			writeFile(BX, CX, DX, AH);
			break;
		case 0x06:
			executeProgram(BX, CX, DX, AH);
			break;
		default:
			printString("Invalid interrupt");
	}
}


// Implementasi fungsi
void printString(char *string) {
	int i = 0;
	while(string[i] != '\0'){
		interrupt(0x10, 0xE00 + string[i], 0, 0, 0);
		i++;
	}
}

void readString(char *string) {
	int i = 0;
	int idx = 0;
	char input;
	do {
		input = interrupt(0x16, 0, 0, 0, 0);
		if (input == '\r') {
			string[idx] = '\0';
			interrupt(0x10, 0xE00 + input, 0, 0, 0);
		} else if (input != '\b') {
			interrupt(0x10, 0xE00 + input, 0, 0, 0);
			string[idx] = input;
			idx++;
		} else if (idx > 0) {
			idx--;
			interrupt(0x10, 0xE00+ '\b', 0, 0, 0);
		}
	} while (input != '\r');
	interrupt(0x10, 0xE00 + '\r', 0, 0, 0);

}

void readSector(char *buffer, int sector) {
	interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
	interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

//read file
//1. while pertama, sampai path nya abis
//baca path nya sampe ketemu / lalu taro setiap directorynya ke array of 

void readFile(char *buffer, char *path, int *result, char parentIndex) {
	char idxParent = parentIndex;
	char files[1024];
	int isFound = 0;
	int isWrongName = 0;
	// int isNameTrue;
	int i = 0;
<<<<<<< HEAD
	int j, k, m, n, h;
	
	
	readSector(files, 0x101); //add files from sector 0x101
	//add files from sector 0x102
	int sConv;
	int n;


	readSector(files, 257);
	readSector(files + 512, 258);
>>>>>>> 5ed467dd3496c9afeece2509b9ce64165b043497
	while(!isFound) {
		j = i;
		// isNameTrue = 0;
		while (path[i] != '/' && path[i] != '\0') {
			i++;
		//finding nemo
		
		//search for parent idx with matching path name
		for (k=0; k < 1024; k+=16) {
			if (files[k] == idxParent) {
				m = k+2;				//matching name
				
				for (h=0; h < i-j-1; h++) {
					if (path[j+h] != files[m+h]) {
						break;
					}
				} if (h == i-j-1) {
					// isNameTrue = 1;
					idxParent = k; //in hexa gengs
					break;
				}
			}
		}
		if (k==1024) break; // break while terluar
		
		if (path[i] == '/') {
			i++;
		} else {
			isFound = 1;
		}
	}

	if (!isFound) *result = -1;
	else {
		//convert idxparent ke int dulu
		// int pConv = convertHexToInt(idxParent);
		s = files[idxParent + 1];
		
		readSector(sectors,259);
		//convert s to int dulu
		sConv = s; //ini belum ya gengs
		n = 0;
		while (sectors[sConv + n] != '\0') {
			buffer[n] = sectors[sConv+n];
			n++;
		}
		*result = 1; //ini masih sementara
	}
}

void clear(char *buffer, int length) { //Fungsi untuk mengisi buffer dengan 0
	int i;
	for(i = 0; i < length; ++i){
		buffer[i] = 0x00;
	}
} 

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
	char map[512];
	char files[1024];
	int i, countSector, entryIndex;
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

// void writeFile(char *buffer, char *filename, int *sectors) {
// 	char map[512];
// 	char dir[512];
// 	char sectorBuffer[512];
// 	int dirIndex;

// 	readSector(map, 1);
// 	readSector(dir, 2);

// 	for (dirIndex = 0; dirIndex < 16; ++dirIndex) {
// 		if (dir[dirIndex * 32] == '\0') {
// 			break;
// 		}
// 	}
// 	if (dirIndex < 16) {
// 		int i, j, sectorCount;
// 		for (i = 0, sectorCount = 0; i < 256 && sectorCount < *sectors; ++i) {
// 			if (map[i] == 0x00) {
// 				++sectorCount;
// 			}
// 		}
// 		if (sectorCount < *sectors) {
// 			*sectors = 0; //insufficient
// 			return;
// 		}
// 		else {
// 			clear(dir + dirIndex * 32, 32);
// 			for (i = 0; i < 12; ++i) {
// 				if (filename[i] != '\0') {
// 					dir[dirIndex * 32 + i] = filename[i];
// 				}
// 				else {
// 					break;
// 				}
// 			}
// 			for (i = 0, sectorCount = 0; i < 256 && sectorCount < *sectors; ++i) {
// 				if (map[i] == 0x00) {
// 					map[i] = 0xFF;
// 					dir[dirIndex * 32 + 12 + sectorCount] = i;
// 					clear(sectorBuffer, 512);
// 					for (j = 0; j < 512; ++j) {
// 						sectorBuffer[j] = buffer[sectorCount * 512 + j];
// 					}
// 					writeSector(sectorBuffer, i);
// 					++sectorCount;
// 				}
// 			}
// 		}	
// 	}
// 	else {
// 		*sectors = -1; //insufficient dir entries
// 		return;
// 	}
	
// 	writeSector(map, 1);
// 	writeSector(dir, 2);
// }

void executeProgram(char *filename, int segment, int *success) {
	char buffer[16 * 512];
	int i;
	readFile(&buffer, filename, success, 0xFF);
	if (*success) {
		for (i=0; i<20 * 512; i++) {
			putInMemory(segment, i, buffer[i]);
		}
		launchProgram(segment);
	} else {
		interrupt(0x21, 0, "File doesn't exist!", 0, 0);
	}
}

void printLogo () {
	printString(" /\\                 /\\\r\n");
	printString("/ \'._   (\\_/)   _.'/ \\\r\n");
	printString("|.''._'--(o.o)--'_.''.|\r\n");
	printString(" \\_ / `;=/ \" \\=;` \\ _/\r\n");
	printString("   `\\__| \\___/ |__/`\r\n");
	printString("jgs     \\(_|_)/\r\n");
	printString("         \" ` \"\r\n");
}

//Implementasi Fungsi Matematika 
int mod(int x, int y) { 
    while (x>=y) {
        x-=y;
    }return x;
}

int div (int x, int y) {
	int ratio = 0;
	while(ratio*y <= x) {
		ratio += 1;
	}return(ratio-1);
}