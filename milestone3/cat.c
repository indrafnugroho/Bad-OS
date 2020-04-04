#include "textmodule.h"
#include "fileIOmodule.h"
#include "mathmodule.h"

void main () {
    char name[14], tempBuff[512], fileContent[512 * 16], currDir;
	int isSuccess, i;
	isSuccess = 0;
	
    // get parentIdx and filename
	interrupt(0x21, 0x02, tempBuff, 0x1F, 0);
    currDir = tempBuff[0];
    for (i = 0; i < 14; i++) {
		name[i] = tempBuff[i + 1];
	}

	//empty Buffer
	for (i = 0; i < 512 * 16; i++) {
		fileContent[i] = 0x0;
	}

	//read fileContent
	readFile(fileContent, name, &isSuccess, currDir);

	if (isSuccess == 1) {
		printString("File content: \r\n\0");
		printString(fileContent);
		printString("\r\n\0");
	} else {
		printString("File not found\r\n\0");
	}
    interrupt(0x21, 0x06, "shell", 0x2000, &isSuccess);
}