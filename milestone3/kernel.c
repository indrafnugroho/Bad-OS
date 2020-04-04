void handleInterrupt21 (int AX, int BX, int CX, int DX);

// text module
#include "textmodule.h"

// system module
void readSector(char *buffer, int sector); 
void writeSector(char *buffer, int sector); 
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void executeProgram(char *filename, int segment, int *success);

// fileIO module
#include "fileIOmodule.h"

// folderIO module
#include "folderIOmodule.h"

// math module
#include "mathmodule.h"

//Main Function
int main() {
	int suc;

	printLogo();
	makeInterrupt21();
	handleInterrupt21(0XFF06, "shell", 0x2000, &suc);
	
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

// system module
void readSector(char *buffer, int sector) {
	interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector) {
	interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void clear(char *buffer, int length) {
	int i;
	for(i = 0; i < length; ++i){
		buffer[i] = 0x00;
	}
}

void executeProgram(char *filename, int segment, int *success) {
	char bufferFile[512 * 16];
	int i;
	
	readFile(&bufferFile, filename, success, 0xFF);
	if (*success) {
		for (i=0; i<512 * 16; i++) {
			putInMemory(segment, i, bufferFile[i]);
		}
		launchProgram(segment);
	} else {
		interrupt(0x21, 0, "File doesn't exist!", 0, 0);
	}
}