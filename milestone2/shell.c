void ls(char parentIndex);
void execProg(char* progName, char parentIndex);
void cat(char parentIndex);
void mkdir(char parentIndex);
int compareStr(char* strA, char* strB);
int compareStrN(char* strA, char* strB, int n);
char searchForPath(char* path, char parentIndex);
char* searchName(char parentIndex);
void getCommand(char* input);

char curdir = 0xFF;

int main() {
	char filename[14];
	char* input;
	int suc, i;

	while (1) {
		do {
			// if (curdir == 0XFF) {
				interrupt(0x21, 0x00, "Root", 0, 0);
			// } else {
			// 	interrupt(0x21, 0x00, "Kenapa ini\r\n", 0, 0);
			// }
			interrupt(0x21, 0x00, ">", 0, 0);
			interrupt(0x21, 0x01, input, 1, 0);
		} while (compareStr(input, ""));
		interrupt(0x21, 0x00, "\r\n", 0, 0);
		// parseInput(input);
		
		// execProg(input, 0xFF);
		
		if (compareStrN(input, "cat", 3)) {
			interrupt(0x21, 0x00, "Cat\r\n", 0, 0);
			cat(0xFF);
		} else if (compareStrN(input, "ls", 2)) {
			interrupt(0x21, 0x00, "ls\r\n", 0, 0);
			ls(0xFF);
		} else if (compareStrN(input, "./", 2)) {
			interrupt(0x21, 0x00, "exec\r\n", 0, 0);
			i = 2;
			while (i < 16 ) {
				if (input[i] == '\0') {
					break;
				} else {
					filename[i - 2] = input[i];
				}
				i++;
			}

			while (i < 16) {
				filename[i-2] = '\0';
				i++;
			}
			interrupt(0x21, 0x00, filename, 0, 0);
			interrupt(0x21, 0x00, "\r\n", 0, 0);

			execProg(filename, 0xFF);
		} else if(compareStrN(input,"mkdir", 5)) {
			interrupt(0x21, 0x00, "mkdir\r\n", 0, 0);
			mkdir(0xFF);
		} else {
			interrupt(0x21, 0x00, "Invalid Command!\r\n", 0, 0);
		}
	}

	return 0;
}

void mkdir(char parentIndex) {
	char directory[16];
	char file[512];
	int i, found, emp;
	for(i=0;i<16;i++) {
		directory[i] = 0x0;
	}

	interrupt(0x21, 0x0,"Nama directory baru : \0", 0, 0);
	interrupt(0x21, 0x1, directory, 0,0);
	interrupt(0x21, 0x2,file,257,0);
	i = 0;
	found = 0;
	for(i;i<64;i++) {
		if(file[i*16] == 0x0 && file[i*16+1] == 0) {
			found = 1;
			emp = i;
		} if(found) {
			break;
		}
	}
	if(found) {
		file[emp*16+1] = 0xFF;
		file[emp*16] = curdir;
		while(i<14) {
			file[emp*16+2+i] = directory[i];
			i++;
		}
	}
	interrupt(0x21,0x3,file,257,0);
	interrupt(0x21, 0x0,"\r\nMakedir Sabi\r\n\0", 0, 0);
}

void execProg(char* progName, char parentIndex) {
	int isFound = 0, k, h, isNameMatch, suc;
	char files[1024];

	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);

	interrupt(0x21, 0x00, progName, 0, 0);
	interrupt(0x21, 0x00, "\r\n", 0, 0);

	k = 0;
	while (k < 64 && !isFound) {
		if (files[k * 16] == 0xFF && files[k * 16 + 1] != 0xFF) {
			// interrupt(0x21, 0x00, "Nyampe parent dir\r\n", 0, 0);
			h = 0;
			isNameMatch = 1;
            for (h = 0; h < 14; h++) {
                if (progName[h] != files[h + k * 16 + 2]) {
	                isNameMatch = 0;
					break;
        	    } else if (files[h + k * 16 + 2] == '\0' && progName[h] == '\0') {
            		break;
            	}
        	}

        	if (isNameMatch) {
        		// interrupt(0x21, 0x00, "anjay nama sama\r\n", 0, 0);
        		isFound = 1;
        	}
		}
		k++;
	}

	if (isFound) {
		interrupt(0x21, 0x06, progName, 0x4000, &isFound);
	} else {
		interrupt(0x21, 0, "Program tidak ditemukan:(\r\n", 0, 0);
	}
}

void ls(char parentIndex) {
	char files[1024];
	int k, h, idxName;
	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);

	k = 0;
	while (k < 64) {
		if (files[k * 16] == parentIndex) {
			h = 0;
			idxName = k * 16 + 2;
			while (h < 14 && files[idxName + h] != '\0') {
				interrupt(0x10, 0xE00 + files[h + idxName], 0, 0, 0);
				h++;
			}
			interrupt(0x21, 0x00, "\r\n\0", 0, 0);
		}
		k++;
	}
}

void cat(char parentIndex) {
	char fileName[14], fileContent[512 * 16];
	int isSuccess, i;
	isSuccess = 0;

	//empty Buffer
	for (i = 0; i < 14; i++) {
		fileName[i] = 0x0;
	}
	
	for (i = 0; i < 512 * 16; i++) {
		fileContent[i] = 0x0;
	}
	
	//read input filename
	interrupt(0x21, 0, "Input filename: \0", 0, 0);
	interrupt(0x21, 1, &fileName, 0, 0);
	interrupt(0x21, 0, "\r\n\0", 0, 0);

	//read fileContent
	interrupt(0x21, (parentIndex << 8) | 0x04, &fileContent, &fileName, &isSuccess);

	if (isSuccess == 1) {
		interrupt(0x21, 0, "File content: \r\n\0", 0, 0);
		interrupt(0x21, 0, &fileContent, 0, 0);
		interrupt(0x21, 0, "\r\n\0", 0, 0);
	} else {
		interrupt(0x21, 0, "File not found\r\n\0", 0, 0);
	}
}

int compareStr(char* strA, char* strB) {
	int i = 0;
	while (!(strA[i] == '\0' && strB[i] == '\0')) {
		if (strA[i] != strB[i]) {
            return 0;
        }
		++i;
	}
	return 1;
}

int compareStrN(char* strA, char* strB, int n) {
	int i = 0;
	while (!(strA[i] == '\0' && strB[i] == '\0') && i < n) {
		if (strA[i] != strB[i]) {
            return 0;
        }
		++i;
	}
	return 1;
}

char searchForPath(char* path, char parentIndex) {
	char files[1024];
	char tempBuffer[512];
	char tempBuffer2[512];
	int isFound = 0;
	int isNameMatch, k, s, j, idxName;
	int h, l;

	interrupt(0x21, 0x00, "Mencari folder: ", 0, 0);
	interrupt(0x21, 0x00, path, 0, 0);
	interrupt(0x21, 0x00, "\r\n", 0, 0);
	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);
	
	k = 0;

	while (isFound == 0 && k < 64) {
		if (files[k * 16] == parentIndex) {
			interrupt(0x21, 0x00, "parentnya ketemu bro\r\n", 0, 0);
			idxName = k * 16 + 2;
			if (files[idxName] != 0x0) {
				// Ini buat baca folder
				interrupt(0x21, 0x00, "ini folder bro\r\n", 0, 0);
				isNameMatch = 1;
				h = 0;
				while (h < 14 && path[h] != '\0') {
					interrupt(0x21, 0x00, path[h], 0, 0);
					interrupt(0x21, 0x00, "\r\n", 0, 0);
					interrupt(0x21, 0x00, files[idxName + h], 0, 0);
					interrupt(0x21, 0x00, "\r\n", 0, 0);
					if (path[h] != files[idxName + h]) {
						interrupt(0x21, 0x00, "salah nama \r\n", 0, 0);
						isNameMatch = 0;
						break;
					}
					h++;
				}
				if (isNameMatch) {
					interrupt(0x21, 0x00, "yes folder ketemu bro\r\n", 0, 0);
					isFound = 1;
					break;
				} 
			} else {
				interrupt(0x21, 0x00, "ganemu folder bro\r\n", 0, 0);
			}
		}

		k += 16;
	}

	if (isFound) {
		parentIndex = k;
	} else {
		interrupt(0x21, 0x00, "yah gaketemu bro\r\n", 0, 0);
		parentIndex = 0x00;
	}

	return parentIndex;

	// while(!isFound) {
	// 	//search for parent idx with matching path name
	// 	for (k; k < 1024; k+=16) {
	// 		if (files[k] == parentIndex) {
	// 			interrupt(0x21, 0x00, "parentnya ketemu bro\r\n", 0, 0);
	// 			idxName = k+2;
	// 			if (files[idxName] != 0x0 && files[k+1] != 0xFF) {
	// 				//matching name
	// 				isNameMatch = 1;
	// 				for (h=0; h < 14; h++) {
	// 					if (path[h] != files[idxName + h]) {
	// 						isNameMatch = 0;
	// 						break;
	// 					}
	// 				} 
					
	// 				if (isNameMatch) {
	// 					isFound = 1;
	// 					s = files[k+1]; //in hexa gengs
	// 					break;
	// 				}
	// 			}
	// 		}
	// 	}
	// 	if (k==1024) {
	// 		interrupt(0x21, 0x00, "k sudah 1024 bro\r\n", 0, 0);

	// 		break; // break while terluar
	// 	}
	// }

	if (!isFound) {
		parentIndex = 0x00;
	}

	interrupt(0x21, 0x00, "bro	 bro\r\n", 0, 0);

	return parentIndex;
}

char* searchName(char parentIndex) {
	char files[1024];
	char name[14];
	
	int isFound = 0;
	int isNameMatch, k, s, j, idxName;
	int h, l;

	interrupt(0x21, 0x2, files, 0x101, 0);
	idxName = parentIndex + 2;
	while (files[idxName] != '\0') {
		name[idxName - 2] = files[idxName];
	}

	return name;
}