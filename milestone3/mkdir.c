// mkdir.c

void main () {
    char directory[14], tempBuff[512], currDir, file[1024];
	int i, found, emp, isSuccess;
	
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
    interrupt(0x21, 0x06, "shell", 0x2000, &isSuccess);
}