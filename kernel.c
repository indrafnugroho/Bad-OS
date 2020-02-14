
#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_FILES 16
#define MAX_FILENAME 12
#define MAX_SECTORS 20
#define DIR_ENTRY_LENGTH 32
#define MAP_SECTOR 1
#define DIR_SECTOR 2
#define TRUE 1
#define FALSE 0
#define INSUFFICIENT_SECTORS 0
#define NOT_FOUND -1
#define INSUFFICIENT_DIR_ENTRIES -1
#define EMPTY 0x00
#define USED 0xFF

/* Ini deklarasi fungsi */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string); //done
void readString(char *string); //done
void readSector(char *buffer, int sector); //done
void writeSector(char *buffer, int sector); //done
void readFile(char *buffer, char *filename, int *success);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0 (done)
void writeFile(char *buffer, char *filename, int *sectors);
void executeProgram(char *filename, int segment, int *success);

//Fungsi Matematika
int mod(int x, int y); //done
int div(int a,int b); //done

//Main Function
int main() {
	char buffer[512 * 20];
	int suc;
	printString("babibu\r\n");
	makeInterrupt21();
	// printLogo();
	//Cari key.txt kalo udah ada
	//Key.txt akan digenerate setelah manggil keyproc
	//Jadi awalnya panggil keyproc dulu karena key.txt ga ada
	//Lalu run lagi programnya, karena sudah ada key.txt, langsung keprint key nya
	interrupt(0x21, 0x4, buffer, "key.txt", &suc);
	printString("bibabu\r\n");
	if (suc)
	{
		printString("tes anj\r\n");
		interrupt(0x21,0x0, "Key : ", 0, 0);
		printString("tes jir\r\n");
	 	interrupt(0x21,0x0, buffer, 0, 0);
		printString("tes ih\r\n");
	}
	else
	{
		printString("tes ah\r\n");
		interrupt(0x21, 0x6, "milestone1", 0x2000, &suc);
		printString("tes oh\r\n");
	}
	// while (1)
	// {

	// }
	// logoPrint(0x0F, 0x03);
	while (1);
}

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
	switch (AX) {
		case 0x0:
			printString(BX);
			break;
		case 0x1:
			readString(BX);
			break;
		case 0x2:
			readSector(BX, CX);
			break;
		case 0x3:
			writeSector(BX, CX);
			break;
		case 0x4:
			readFile(BX, CX, DX);
			break;
		case 0x5:
			writeFile(BX, CX, DX);
			break;
		case 0x6:
			executeProgram(BX, CX, DX);
			break;
		default:
			printString("Invalid interrupt");
	}
}

// Implementasi fungsi
void printString(char *string) { //WORK!!!
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

// void readFile(char *buffer, char *filename, int *success) {
// 	char dir[SECTOR_SIZE];
// 	char entry[DIR_ENTRY_LENGTH];
// 	int fileFound;
// 	int i;
// 	int j;
// 	int ij;
// 	readSector(dir, DIR_SECTOR);
// 	for (i = 0; i < SECTOR_SIZE; i+=DIR_ENTRY_LENGTH) {
// 		fileFound = 1;
// 		for (ij = 0; ij < MAX_FILENAME; ij++) {
// 			if (dir[i + ij] != filename[ij]) {
// 				fileFound = 0;
// 			}
// 		}
// 		if (fileFound == 1) {
// 			for (j = 0; j < DIR_ENTRY_LENGTH; j++) {
// 				entry[j] = dir[i+j];
// 			}
// 			break;
// 		}
// 	}
// 	if (fileFound == 0) {
// 		*success = 0;
// 		return;
// 	} else {
// 		int k = 0;
// 		int search_byte = -999;
// 		while ((k < MAX_SECTORS) && (search_byte != 0)) {
// 			readSector(buffer + k * SECTOR_SIZE, entry[k]);
// 			k++;
// 		}
// 		*success = 1;
// 		return;
// 	}

// }

void readFile(char *buffer, char *filename, int *success) {
	char dir[SECTOR_SIZE];
	int iterDir = 0;
	int iterFileName;
	char ketemu = FALSE;
	char sama;
	int iterLastByte, i;
	//Isi dir dengan list of semua filename
	readSector(dir, DIR_SECTOR);
	//Traversal dir
	for (iterDir = 0; iterDir < SECTOR_SIZE; iterDir += DIR_ENTRY_LENGTH) {
		sama = TRUE;
		for (iterFileName = 0; iterFileName < MAX_FILENAME; iterFileName++) {
			if (filename[iterFileName] == '\0') {
				break;
			}
			else {
				if (filename[iterFileName] != dir[iterDir + iterFileName]) {
					sama = FALSE;
					break;
				}
			}
		}
		if (sama) {
			ketemu = TRUE;
			break;
		}
	}
	//Cek apakah sudah ketemu
	if (!ketemu) {
		*success = FALSE;
		return;
	}
	else {
		//Traversal 20 byte terakhir dari dir[iterDir] - dir[iterDir+32]
		iterLastByte = iterDir + MAX_FILENAME;
		for (i = 0; i < MAX_SECTORS; i++) {
			if (dir[iterLastByte + i] == 0) {
				break;
			}
			else {
				readSector(buffer + i * SECTOR_SIZE, dir[iterLastByte + i]);
			}
		}
		*success = TRUE;
		return;
	}
}

void clear(char *buffer, int length) { //Fungsi untuk mengisi buffer dengan 0
	int i;
	for(i = 0; i < length; ++i){
		buffer[i] = 0x00;
	}
} 

void writeFile(char *buffer, char *filename, int *sectors) {
	char map[SECTOR_SIZE];
	char dir[SECTOR_SIZE];
	char sectorBuffer[SECTOR_SIZE];
	int dirIndex;

	readSector(map, MAP_SECTOR);
	readSector(dir, DIR_SECTOR);

	for (dirIndex = 0; dirIndex < MAX_FILES; ++dirIndex) {
		if (dir[dirIndex * DIR_ENTRY_LENGTH] == '\0') {
			break;
		}
	}
	if (dirIndex < MAX_FILES) {
		int i, j, sectorCount;
		for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
			if (map[i] == EMPTY) {
				++sectorCount;
			}
		}
		if (sectorCount < *sectors) {
			*sectors = INSUFFICIENT_SECTORS;
			return;
		}
		else {
			clear(dir + dirIndex * DIR_ENTRY_LENGTH, DIR_ENTRY_LENGTH);
			for (i = 0; i < MAX_FILENAME; ++i) {
				if (filename[i] != '\0') {
					dir[dirIndex * DIR_ENTRY_LENGTH + i] = filename[i];
				}
				else {
					break;
				}
			}
			for (i = 0, sectorCount = 0; i < MAX_BYTE && sectorCount < *sectors; ++i) {
				if (map[i] == EMPTY) {
					map[i] = USED;
					dir[dirIndex * DIR_ENTRY_LENGTH + MAX_FILENAME +
					sectorCount] = i;
					clear(sectorBuffer, SECTOR_SIZE);
					for (j = 0; j < SECTOR_SIZE; ++j) {
						sectorBuffer[j] = buffer[sectorCount * SECTOR_SIZE + j];
					}
					writeSector(sectorBuffer, i);
					++sectorCount;
				}
			}
		}	
	}
	else {
		*sectors = INSUFFICIENT_DIR_ENTRIES;
		return;
	}
	
	writeSector(map, MAP_SECTOR);
	writeSector(dir, DIR_SECTOR);
}

void executeProgram(char *filename, int segment, int *success) {
	char buffer[MAX_SECTORS * SECTOR_SIZE];
	int i;
	readFile(buffer, filename, success);
	if (*success) {
		for (i=0; i<MAX_SECTORS * SECTOR_SIZE; i++) {
			putInMemory(segment, i, buffer[i]);
		}
		launchProgram(segment);
	}
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

