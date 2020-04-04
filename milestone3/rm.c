#include "textmodule.h"
#include "fileIOmodule.h"
#include "folderIOmodule.h"

void main() {
    char name[14], folderAndFiles[512*2], tempBuff[512], currDir;
	int i, j, isFound, isNameMatch, idxName, folderFilesEntry, isSuccess;
	isFound = 0;

	// get parentIdx and filename
	interrupt(0x21, 0x02, tempBuff, 512, 0);
    currDir = tempBuff[0];
    for (i = 0; i < 14; i++) {
		name[i] = tempBuff[i + 1];
	}

	//read sector 257 and 258 (file/folder)
	interrupt(0x21, 0x02, folderAndFiles, 257, 0);
	interrupt(0x21, 0x02, folderAndFiles + 512, 258, 0);
	
	i = 0;
	while (!isFound && i < 1024) {
        // Search for files / folder
        for (i = 0; i < 1024; i += 16) {
			if (folderAndFiles[i] == currDir) {
				if (folderAndFiles[i + 2] != 0x0) {
                	idxName = i + 2;
                
                	//matching name
                	isNameMatch = 1;
	                for (j = 0; j < 14; j++) {
    	                if (name[j] != folderAndFiles[j + idxName]) {
        	                isNameMatch = 0;
							break;
                	    } else if (folderAndFiles[j + idxName] == '\0' && name[j] == '\0') {
                    		break;
                    	}
                	}

                	if (isNameMatch) {
                    	isFound = 1;
                    	folderFilesEntry = folderAndFiles[i + 1];
						break;
                	}
				}
        	}
		}
    }
	
	if (!isFound) {
		printString("File/folder not found\r\n\0");
	}
	else {
		if (folderFilesEntry == 0xFF) {
			delDir(folderFilesEntry);
			printString("Folder deleted successfully!\r\n\0");
		} else {
			delFile(folderFilesEntry);
			printString("File deleted successfully!\r\n\0");
		}
	}
    interrupt(0x21, 0x06, "shell", 0x2000, &isSuccess);
}