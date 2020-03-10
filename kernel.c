
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

void readFile(char *buffer, char *filename, int *success) {
	char dir[SECTOR_SIZE];
	char entry[32];
	int fileFound;
	int i;
	int j;
	int ij;
	int startLast20Bytes;
	int k;
	char check = 0;
	readSector(dir, 2);
	for (i = 0; i < 512; i+=32) {
		fileFound = 1;
		for (ij = 0; ij < 12; ij++) {
			if (filename[ij] == '\0') {
				break;
			}
			else if (dir[i + ij] != filename[ij]) {
				fileFound = 0;
				break;
			}
		}
		if (fileFound) {
			check = 1;
			break;
		}
	}
	if (!check) {
		*success = 0;
		return;
	} else {
		startLast20Bytes = i + 12;
		for (k = 0; k < 20; k++) {
			if (dir[startLast20Bytes + k] == 0) {
				break;
			} else {
				readSector(buffer + k * SECTOR_SIZE, dir[startLast20Bytes + k]);
			}
		}
		*success = 1;
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

