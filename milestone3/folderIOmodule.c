#include "folderIOmodule.h"

// make dir

void delDir(char entry) {
	char folderAndFiles[1024];
	int i;

	interrupt(0x21, 0x02, folderAndFiles, 257, 0);
	interrupt(0x21, 0x02, folderAndFiles + 512, 258, 0);

	//delete folder entry
	folderAndFiles[entry * 16] = 0x0;
	folderAndFiles[entry * 16 + 1] = '\0';

	//rewrite buffer to sector
	interrupt(0x21, 0x03, &folderAndFiles, 257, 0);
	interrupt(0x21, 0x03, folderAndFiles + 512, 258, 0);

	for (i = 0; i < 64; i++) {
		if (folderAndFiles[i * 16] != 0xFF) {
			if (folderAndFiles[i * 16] == entry && folderAndFiles[i * 16 + 1] != '\0') {
	 			delFile(i);
	 		}
		}
	}

	for (i = 0; i < 64; i++) {
		if (folderAndFiles[i * 16] == entry && folderAndFiles[i * 16 + 1] == 0xFF) {
			delDir(i);
	 	}
	}
}

// list content