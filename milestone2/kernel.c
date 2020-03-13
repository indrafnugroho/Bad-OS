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
	char buffer[512 * 20];
	int suc;
	printLogo();
	makeInterrupt21();
	interrupt(0x21, 0x4, buffer, "key.txt", &suc);
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
			Break;
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
	readSector(files, 257);
	
	int isFound = 0;
	int isWrongName = 0;
	int isNameTrue;
	int i = 0;
	int j;
	while(!isFound) {
		j = i;
		isNameTrue = 0;
		while (path[i] != '/' && path[i] != '\0') {
			i++;
		}
		//finding nemo
		int k;
		//search for parent idx with matching path name
		for (k=0; k < 1024; k+=16) {
			if (files[k] == idxParent) {
				int m = k+2;
				//matching name
				int h;
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
		
		if (path[i] == '/') {
			i++;
		} else {
			isFound = 1;
		}
	}

	if (!isFound) *result = -1;
	else {
		//convert idxparent ke int dulu
		int pConv = convertHexToInt(idxParent);
		char s = files[pConv + 1];
		char sectors[512];
		readSector(sectors,259);
		//convert s to int dulu
		int sConv = convertHexToInt(s); //ini belum ya gengs
		int n = 0;
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

// void writeFile(char *buffer, char *path, int *sectors, char parentIndex);

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
	char buffer[20 * 512];
	int i;
	readFile(buffer, filename, success);
	if (*success) {
		for (i=0; i<20 * 512; i++) {
			putInMemory(segment, i, buffer[i]);
		}
		launchProgram(segment);
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