// #include "folderIOmodule.h"

void makeDir() {
	char directory[14], tempBuff[512], file[1024], currDir;
	int i, found, emp;
	
	// get parentIdx and filename
	interrupt(0x21, 0x02, tempBuff, 512, 0);
    currDir = tempBuff[0];
    for (i = 0; i < 14; i++) {
		directory[i] = tempBuff[i + 1];
	}

	interrupt(0x21, 0x2, file, 0x101, 0);
	interrupt(0x21, 0x2, file + 512, 0x102, 0);
	
    i = 0;
	found = 0;
	while(i < 64 && !found) {
		if(file[i * 16] == 0x0 && file[i * 16 + 1] == 0) {
			found = 1;
			emp = i;
			break;
		}
		++i;
	}
	
    if(found) {
		file[emp * 16] = currDir;
		file[emp * 16 + 1] = 0xFF;
		for(i = 0; i < 14; i++) {
			file[emp * 16 + 2 + i] = directory[i];
		}
		interrupt(0x21, 0x3, file, 257, 0);
	}
}

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
				printString("(dir)  ");
			} else {
				printString("(file) ");
			}
			while (h < 14 && files[idxName + h] != '\0') {
				interrupt(0x10, 0xE00 + files[h + idxName], 0, 0, 0);
				h++;
			}
			printString("\r\n\0");
			countDir++;
		}
		k++;
	}
	if (countDir == 0) {
		printString("Empty Directory\r\n\0");
	}
}