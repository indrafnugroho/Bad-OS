void ls(char parentIndex);
void execProg(char* progName, char parentIndex);
void cat(char parentIndex);
void mkdir(char* arg);
int cd(char* cmd, int idxDir);
void rm(char parentIndex);
unsigned char compareStr(char* strA, char* strB);
int compareStrN(char* strA, char* strB, int n);
int searchPath(char* dirCall, int parentIndex);
char searchForPath(char* path, char parentIndex);
char* searchName(char parentIndex);
void getCommand(char* input);

int curdir, dirBack, dirChange;
curdir = 0xFF;

int main() {
	char arg[14], directoryBuffer[1024], curDirName[128];
	char* input;
	int suc, i, itrDirName;
	curdir = 0xFF;
	itrDirName = 0;
	dirChange = 0;
	dirBack = 0;

	i = 0;
	while (i < 128) {
		curDirName[i] = '\0';
		i++;
	}
	i = 0;

	while (1) {
		do {
			interrupt(0x21, 0x00, "Root", 0, 0);
			if (dirChange) {
				if (dirBack == 0) {
					interrupt(0x21, 0x2, directoryBuffer, 0x101, 0);
					interrupt(0x21, 0x2, directoryBuffer + 512, 0x102, 0);
					// interrupt(0x21, 0x00, "abis readdir\r\n", 0, 0);

					curDirName[itrDirName++] = '/';
					i = 0;
					while (i < 14 ) {
						if (directoryBuffer[curdir * 16 + 2 + i] == '\0') {
							// interrupt(0x21, 0x00, "bye loop\r\n", 0, 0);
							break;
						} else {
							// interrupt(0x21, 0x00, "masukkin char\r\n", 0, 0);
							curDirName[itrDirName + i] = directoryBuffer[i + curdir * 16 + 2];
							i++;
						}
					}
					itrDirName += i;
					dirChange = 0;
				} else {
					while (!(curDirName[itrDirName] == '/')) {
						curDirName[itrDirName--] = '\0';
					}
					curDirName[itrDirName] = '\0';
					dirBack = 0;
				}
			}
			interrupt(0x21, 0x00, curDirName, 0, 0);
			interrupt(0x21, 0x00, ">", 0, 0);
			interrupt(0x21, 0x01, input, 1, 0);
		} while (compareStr(input, ""));
		interrupt(0x21, 0x00, "\r\n", 0, 0);
		
		if (compareStrN(input, "cat", 3)) {
			interrupt(0x21, 0x00, "Cat\r\n", 0, 0);
			cat(curdir);
		} else if (compareStrN(input, "ls", 2)) {
			interrupt(0x21, 0x00, "ls\r\n", 0, 0);
			ls(curdir);
		} else if (compareStrN(input, "rm", 2)) {
			interrupt(0x21, 0x00, "rm is being run\r\n\0", 0, 0);
			rm(curdir);
		} else if (compareStrN(input, "./", 2)) {
			// interrupt(0x21, 0x00, "masuk if exec\r\n", 0, 0);
			i = 2;
			while (i < 16 ) {
				if (input[i] == '\0') {
					break;
				} else {
					arg[i - 2] = input[i];
				}
				i++;
			}

			while (i < 16) {
				arg[i-2] = '\0';
				i++;
			}
			interrupt(0x21, 0x00, arg, 0, 0);
			interrupt(0x21, 0x00, "\r\n", 0, 0);

			execProg(arg, 0xFF);
		} else if(compareStrN(input,"mkdir",5)) {
			i = 6;
			while (i < 20 ) {
				if (input[i] == '\0') {
					break;
				} else {
					arg[i - 6] = input[i];
				}
				i++;
			}

			while (i < 20) {
				arg[i-6] = '\0';
				i++;
			}

			// interrupt(0x21, 0x00, "Dir yang dibuat:", 0, 0);
			// interrupt(0x21, 0x00, filename, 0, 0);
			mkdir(arg);
		} else if(compareStrN(input,"cd", 2)) {
			i = 3;
			while (i < 17 ) {
				if (input[i] == '\0') {
					break;
				} else {
					arg[i - 3] = input[i];
				}
				i++;
			}

			while (i < 17) {
				arg[i-3] = '\0';
				i++;
			}

			// interrupt(0x21, 0x00, "Dir yang dituju:", 0, 0);
			// interrupt(0x21, 0x00, arg, 0, 0);
			// interrupt(0x21, 0x00, "\r\n", 0, 0);
			curdir = cd(arg, curdir);
		} else {
			interrupt(0x21, 0x00, "Invalid Command!\r\n", 0, 0);
		}
	}

	return 0;
}

int cd(char* cmd, int idxDir) {
	char directory[14];
	int i, cnt, val, cont, initDir;
	cnt = 0;
	cont = 1;
	i=0;
	initDir = idxDir;
	for(i;i<14;++i) {
		directory[i] = '\0';
	}
	i = 0;
	while(i<128 && cmd[i] != '\0' && cont == 1) {
		if(cmd[i] != '/') {
			directory[cnt] = cmd[i];
			cnt++;
		} else if(cmd[i] == '/') {
			val = searchPath(directory, initDir);
			if(val == 0x100) {
				// interrupt(0x21, 0, "Folder ga ketemu bro A!\r\n",0,0);
				// interrupt(0x21, 0, directory,0,0);
				// interrupt(0x21, 0, "\r\n\0", 0, 0);
				cont = 0;
			} else {
				// interrupt(0x21, 0, "Folder ketemu bro A!\r\n",0,0);
				// interrupt(0x21, 0, directory,0,0);
				// interrupt(0x21, 0, "\r\n\0", 0, 0);
				initDir = val;
			}
			cnt = 0;
		}
		++i;
	}
	if(cont) {
		// interrupt(0x21, 0, "Masuk B!\r\n",0,0);
		// interrupt(0x21, 0, directory,0,0);
		// interrupt(0x21, 0, "\r\n",0,0);
		val = searchPath(directory, initDir);
		if(val == 0x100) {
			// interrupt(0x21, 0, "Folder ga ketemu bro B!\r\n",0,0);
			// interrupt(0x21, 0, directory,0,0);
			// interrupt(0x21, 0, "\r\n\0", 0, 0);
			cont = 0;
		} else {
			// interrupt(0x21, 0, "Folder ketemu bro A!\r\n",0,0);
			// interrupt(0x21, 0, directory,0,0);
			// interrupt(0x21, 0, "\r\n\0", 0, 0);
			initDir = val;
			dirChange = 1;
		}
		cnt = 0;
		// for(i;i<14; ++i) {
		// 	directory[i] = 0;
		// }
	}

	return initDir;
}

int searchPath(char* dirCall, int parentIndex) {
	char directoryBuffer[1024], filename[14];
	int i, found, var, h, isNameMatch, k;
	i=0;
	// interrupt(0x21, 0, "Masuk searchPath\r\n", 0, 0);
	// for(i;i<1024;++i) {
	// 	directoryBuffer[i] = '\0';
	// }
	interrupt(0x21, 2, directoryBuffer, 0x101, 0);
	interrupt(0x21, 2, directoryBuffer+512, 0x102, 0);
	found = 0;
	if(dirCall[0] == '.' && dirCall[1] == '.') {
		// interrupt(0x21, 0, "wah cd .. bro\r\n", 0, 0);
		if(parentIndex != 0xFF) {
			var = directoryBuffer[parentIndex*16];
			dirBack = 1;
		} else {
			var = 0xFF;
		}
		found = 1;
	} else {
		// interrupt(0x21, 0, "Oke cd folder\r\n", 0, 0);
		i = 0;
		while (found == 0 && i < 64) {
			// interrupt(0x21, 0, "Oke cd folder\r\n", 0, 0);
			if(directoryBuffer[i*16] == parentIndex && directoryBuffer[16 * i+1] == 0xFF){
				// interrupt(0x21, 0, "Folder ketemu di loop\r\n",0,0);
				// interrupt(0x21, 0, directoryBuffer + i*16 +2,0,0);
				h = 0;
				isNameMatch = 1;
				for (h = 0; h < 14; h++) {
					if (dirCall[h] != directoryBuffer[h + i * 16 + 2]) {
						isNameMatch = 0;
						break;
					} else if (directoryBuffer[h + i * 16 + 2] == '\0' && dirCall[h] == '\0') {
						break;
					}
				}

				if (isNameMatch) {
					var = i;
					found = 1;
					k = 0;
					while (k < 14 ) {
						if (directoryBuffer[i * 16 + 2 + k] == '\0') {
							break;
						} else {
							filename[k] = directoryBuffer[i * 16 + 2 + k];
						}
						k++;
					}

					while (k < 16) {
						filename[k] = '\0';
						k++;
					}
				}
			}
			++i;
 		}	
	}
	if(found) {
		// interrupt(0x21, 0, "Folder ketemu di akhir\r\n",0,0);
		// interrupt(0x21, 0, filename,0,0);
		// interrupt(0x21, 0, "\r\n",0,0);
		return var;
	} else {
		// interrupt(0x21, 0, "Folder tidak ketemu searchPath\r\n",0,0);
		return 0x100;
	}
}

void mkdir(char* arg) {
	char directory[14];
	char file[1024];
	int i, found, emp;
	for(i=0;i<14;i++) {
		directory[i] = arg[i];
	}

	// interrupt(0x21, 0x0,"Nama directory baru : \0", 0, 0);
	// interrupt(0x21, 0x1, directory, 0,0);
	// interrupt(0x21, 0x0,"\r\n", 0, 0);
	interrupt(0x21, 0x2, file,0x101,0);
	interrupt(0x21, 0x2, file + 512,0x102,0);
	i = 0;
	found = 0;
	while(i<64 && !found) {
		if(file[i*16] == 0x0 && file[i*16+1] == 0) {
			found = 1;
			emp = i;
			break;
		}
		++i;
	}
	if(found) {
		file[emp*16] = curdir;
		file[emp*16+1] = 0xFF;
		for(i=0;i<14;i++) {
			file[emp*16+2+i] = directory[i];
		}
		interrupt(0x21, 0x3,file,257,0);
		// interrupt(0x21, 0x0,"\r\nMakedir Sabi\r\n", 0, 0);
	} else {
		// interrupt(0x21, 0x0,"\r\nMakedir gasabi\r\n", 0, 0);
	}
}

void execProg(char* progName, char parentIndex) {
	int isFound = 0, k, h, isNameMatch, suc;
	char files[1024];

	interrupt(0x21, 0, "masuk execProg:(\r\n", 0, 0);

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
	int k, h, idxName, countDir;
	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);

	k = 0;
	countDir = 0;
	while (k < 64) {
		if (files[k * 16] == parentIndex) {
			h = 0;
			idxName = k * 16 + 2;
			if (files[k * 16 + 1] == 0XFF) {
				interrupt(0x21, 0x00, "(dir)  ", 0, 0);
			} else {
				interrupt(0x21, 0x00, "(file) ", 0, 0);
			}
			while (h < 14 && files[idxName + h] != '\0') {
				interrupt(0x10, 0xE00 + files[h + idxName], 0, 0, 0);
				h++;
			}
			interrupt(0x21, 0x00, "\r\n\0", 0, 0);
			countDir++;
		}
		k++;
	}
	if (countDir == 0) {
		interrupt(0x21, 0x00, "Empty Directory\r\n\0", 0, 0);
	}
}

void cat(char parentIndex) {
	char fileName[14], fileContent[512 * 16];
	int isSuccess, i;
	isSuccess = 0;
	interrupt(0x21, 0, "Masuk cat\r\n\0", 0, 0);

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
		interrupt(0x21, 0, "File not founddddddddd\r\n\0", 0, 0);
	}
}

void rm(char parentIndex) {
	char fileName[14], mapBuffer[512], folderAndFiles[512*2], tempBuffer[512], sectBuffer[512];
	int i, j, k, l, isFound, isNameMatch, idxName, fileEntry;
	isFound = 0;

	//empty Buffer
	for (i = 0; i < 14; i++) {
		fileName[i] = 0x0;
	}
	
	//read input filename to be deleted
	interrupt(0x21, 0, "Input filename: \0", 0, 0);
	interrupt(0x21, 1, &fileName, 0, 0);
	interrupt(0x21, 0, "\r\n\0", 0, 0);

	//search for file/folder
	interrupt(0x21, 0x02, &folderAndFiles, 257, 0);
	interrupt(0x21, 0x02, folderAndFiles + 512, 258, 0);
	
	i = 0;
	while (!isFound && i < 1024) {
        // Search for parent idx w/ matching path name
        for (i = 0; i < 1024; i += 16) {
			//for files
			if (folderAndFiles[i] == parentIndex) {
            	if (folderAndFiles[i + 1] != 0xFF && folderAndFiles[i + 2] != 0x0) {
                	idxName = i + 2;
                
                	//matching name
                	isNameMatch = 1;
	                for (j = 0; j < 14; j++) {
    	                if (fileName[j] != folderAndFiles[j + idxName]) {
        	                isNameMatch = 0;
							break;
                	    } else if (folderAndFiles[j + idxName] == '\0' && fileName[j] == '\0') {
                    		break;
                    	}
                	}

                	if (isNameMatch) {
                    	isFound = 1;
                    	fileEntry = folderAndFiles[i + 1];
						break;
                	}
            	} else {
					//haven't got any clue yet on how to handle deleting dir
				}
        	}
		}
    }
	
	if (!isFound) {
		interrupt(0x21, 0, "File/folder not found\r\n\0", 0, 0);
	}
	else {
		//delete file entry
		folderAndFiles[fileEntry * 16] = 0x0;
		folderAndFiles[fileEntry * 16 + 1] = '\0';
		
		//deletion in map and sector
		interrupt(0x21, 0x02, &mapBuffer, 256, 0);
		interrupt(0x21, 0x02, &sectBuffer, 259, 0);
		k = 0;
		while (sectBuffer[fileEntry * 16 + k] != '\0' && k < 16) {
			//make it empty
			mapBuffer[sectBuffer[fileEntry * 16 + k]] = 0x0;
			sectBuffer[fileEntry * 16 + k] = 0x0;
			k++;
		}

		//rewrite buffer to sectors
		interrupt(0x21, 0x03, &folderAndFiles, 257, 0);
		interrupt(0x21, 0x03, folderAndFiles + 512, 258, 0);
		interrupt(0x21, 0x03, &sectBuffer, 259, 0);
		interrupt(0x21, 0x03, &mapBuffer, 256, 0);
		interrupt(0x21, 0, "File/folder deleted successfully!\r\n\0", 0, 0);
	}
}

unsigned char compareStr(char* strA, char* strB) {
	int i = 0;
	unsigned char sama = 1;
	while (i < 128 && sama) {
		if(strA[i] != strB[i]) {
			sama = 0;
		} else if(strA[i] == 0x0 && strB[i] == 0x0) {
			i = 128;
		}
		++i;
	}
	return sama;
	
	// i = 0;
	// while (!(strA[i] == '\0' && strB[i] == '\0')) {
	// 	if (strA[i] != strB[i]) {
    //         return 0;
    //     }
	// 	++i;
	// }
	// return 1;
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