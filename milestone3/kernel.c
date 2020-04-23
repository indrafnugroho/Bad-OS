void handleInterrupt21 (int AX, int BX, int CX, int DX);

// system module
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void executeProgram(char *filename, int segment, int *success);

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
void clear(char *buffer, int length) {
	int i;
	for(i = 0; i < length; ++i){
		buffer[i] = 0x00;
	}
}

void executeProgram(char *filename, int segment, int *success) {
	char bufferFile[512 * 16];
	int i;
	
	clear(bufferFile, 512 * 16);
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