void ls(char parentIndex);
void execProg(char* progName, char parentIndex);
int cd(char* cmd, int idxDir);
unsigned char compareStr(char* strA, char* strB);
int compareStrN(char* strA, char* strB, int n);
int searchPath(char* dirCall, int parentIndex);
void getCommand(char* input);
void executeBin(char* cmd);

int curdir, dirBack, dirChange, itrDirName;
char curDirName[128], directoryBuffer[1024];
curdir = 0xFF;

int main() {
	char arg[14], dirAndName[512], destDir[14];
	char* input;
	int isSuccess, i;
	curdir = 0xFF;
	itrDirName = 0;
	dirChange = 0;
	dirBack = 0;
	
	for (i = 0; i < 512; i++) {
		dirAndName[i] = 0x00;
	}

	i = 0;
	while (i < 128) {
		curDirName[i] = '\0';
		i++;
	}
	i = 0;

	while (1) {
		do {
			interrupt(0x21, 0x2, directoryBuffer, 0x101, 0);
			interrupt(0x21, 0x2, directoryBuffer + 512, 0x102, 0);
			interrupt(0x21, 0x00, "Root", 0, 0);
			if (!(dirBack == 0)) {
				while (!(curDirName[itrDirName] == '/')) {
					curDirName[itrDirName--] = '\0';
				}
				curDirName[itrDirName] = '\0';
				dirBack = 0;
				
			}
			interrupt(0x21, 0x00, curDirName, 0, 0);
			interrupt(0x21, 0x00, ">", 0, 0);
			interrupt(0x21, 0x01, input, 1, 0);
		} while (compareStr(input, ""));
		interrupt(0x21, 0x00, "\r\n", 0, 0);
		
		if (compareStrN(input, "cat", 3)) {
			i = 4;
			while (i < 18) {
				if (input[i] == '\0') break;
				else arg[i - 4] = input[i];
				i++;
			}

			while (i < 18) {
				arg[i-4] = '\0';
				i++;
			}

			dirAndName[0] = curdir;
			for (i = 0; i < 14; i++) {
				dirAndName[i + 1] = arg[i];
			}

			interrupt(0x21, 0x03, dirAndName, 512, 0);
			interrupt(0x21, 0x06, "cat", 0x2000, &isSuccess);
		} else if (compareStrN(input, "ls", 2)) {
			// interrupt(0x21, 0x00, "ls\r\n", 0, 0);
			ls(curdir);
		} else if (compareStrN(input, "rm", 2)) {
			i = 3;
			while (i < 17) {
				if (input[i] == '\0') break;
				else arg[i - 3] = input[i];
				i++;
			}

			while (i < 17) {
				arg[i-3] = '\0';
				i++;
			}

			dirAndName[0] = curdir;
			for (i = 0; i < 14; i++) {
				dirAndName[i + 1] = arg[i];
			}

			interrupt(0x21, 0x03, dirAndName, 512, 0);
			interrupt(0x21, 0x06, "rm", 0x2000, &isSuccess);
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

			dirAndName[0] = curdir;
			for (i = 0; i < 14; i++) {
				dirAndName[i + 1] = arg[i];
			}

			interrupt(0x21, 0x03, dirAndName, 512, 0);
			interrupt(0x21, 0x06, "mkdir", 0x2000, &isSuccess);
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

			curdir = cd(arg, curdir);
		} else if(compareStrN(input,"mv", 2)) {
			i = 3;
			dirAndName[0] = curdir;
			while (input[i] != '\0') {
				dirAndName[i - 2] = input[i];
				i++;
			}

			interrupt(0x21, 0x03, dirAndName, 512, 0);
			interrupt(0x21, 0x06, "mv", 0x2000, &isSuccess);
		} else {
			interrupt(0x21, 0x00, "Invalid Command!\r\n", 0, 0);
			executeBin(input);
		}
	}

	return 0;
}

void executeBin(char* cmd) {
	char files[1024];
	char cache[8192];
	int i, j, isMatch, sec;
	for(i=0;i<1024;++i) {
		files[i] = '\0';
	}
	interrupt(0x21, 0x2, files, 0x101, 0);
	interrupt(0x21, 0x2, files + 512, 0x102, 0);
	isMatch = 0;
	i = 0;
	while(isMatch == 0 && i < 64) {
		if( files[i*16] == 0xFF&&files[16*i] == 0x01) {
			interrupt(0x21, 0, "File ketemu bro! Nama filenya : \0", 0, 0);
			interrupt(0x21, 0, files + (i*16) + 2, 0, 0);
			if(compareStr(files + i*16 + 2, cmd)) {
				interrupt(0x21, 0, "File cocok!\r\n\0",0,0);
				isMatch = 1;
			}
		}
		++i;
	}
	if(isMatch == 1) {
		execProg(cmd, curdir);
	} else{
		interrupt(0x21, 0, "Gada file kek gitu udahh..\r\n\0",0,0);
	}
}

int cd(char* cmd, int idxDir) {
	char directory[14];
	int i, cnt, val, cont, initDir, k;
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
				interrupt(0x21, 0, "Masuk sini bro!\r\n",0,0);
				// interrupt(0x21, 0, directory,0,0);
				// interrupt(0x21, 0, "\r\n\0", 0, 0);
				initDir = val;
				curDirName[itrDirName++] = '/';
				k = 0;
				while (k < 14 ) {
					if (directoryBuffer[initDir * 16 + 2 + k] == '\0') {
						// interrupt(0x21, 0x00, "bye loop\r\n", 0, 0);
						break;
					} else {
						// interrupt(0x21, 0x00, "masukkin char\r\n", 0, 0);
						curDirName[itrDirName + k] = directoryBuffer[k + initDir * 16 + 2];
						k++;
					}
				}
				itrDirName += k;
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
			interrupt(0x21, 0, "Masuk sini bro2!\r\n",0,0);
				// interrupt(0x21, 0, directory,0,0);
				// interrupt(0x21, 0, "\r\n\0", 0, 0);
			if (dirBack ==0) {
				initDir = val;
				curDirName[itrDirName++] = '/';
				k = 0;
				while (k < 14 ) {
					if (directoryBuffer[initDir * 16 + 2 + k] == '\0') {
						// interrupt(0x21, 0x00, "bye loop\r\n", 0, 0);
						break;
					} else {
						// interrupt(0x21, 0x00, "masukkin char\r\n", 0, 0);
						curDirName[itrDirName + k] = directoryBuffer[k + initDir * 16 + 2];
						k++;
					}
				}
				itrDirName += k;
			}
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