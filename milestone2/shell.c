int compareStr(char* strA, char* strB);
char searchForPath(char* path, char parentIndex);
char* searchName(char parentIndex);

int main() {
	char curdir;
	char* input;

	curdir = 0xFF;
	while (1) {
		do {
			if (curdir = 0XFF) {
				interrupt(0x21, 0x00, "Root", 0, 0);
			} else {
				interrupt(0x21, 0x00, "Kenapa ini\r\n", 0, 0);
			}
			interrupt(0x21, 0x00, ">", 0, 0);
			interrupt(0x21, 0x01, input, 1, 0);
		} while (compareStr(input, ""));

		interrupt(0x21, 0x00, "\r\n", 0, 0);
		curdir = searchForPath(&input, 0xFF);
		interrupt(0x21, 0x00, curdir, 0, 0);
		interrupt(0x21, 0x00, "\r\n", 0, 0);
	}

	return 0;
}

int compareStr(char* strA, char* strB) {
	int i = 0;
	while (!(strA[i] == '\0' && strB[i] == '\0')) {
		if (strA[i] != strB[i]) {
            return 0;
        }
		++i;
	}
	return 1;
}

char searchForPath(char* path, char parentIndex) {
	char files[1024];
	char tempBuffer[512];
	char tempBuffer2[512];
	int isFound = 0;
	int isNameMatch, k, s, j, idxName;
	int h, l;

	interrupt(0x21, 0x2, files, 0x101, 0);
	for (k=512; k < 1024; k++) {
		files[k] = tempBuffer[k-512];
	}
	
	k = 0;
	while(!isFound && k < 1024) {
		//search for parent idx with matching path name
		for (k = 0; k < 1024; k += 16) {
			if (files[k] == parentIndex) {
				idxName = k + 2;
				if (files[idxName] != 0x0 && files[k + 1] != 0xFF) {
					//matching name
					isNameMatch = 1;
					for (h = 0; h < 14; h++) {
						if (path[h] != files[idxName + h]) {
							isNameMatch = 0;
							break;
						}
					} 
					
					if (isNameMatch) {
						isFound = 1;
						s = files[k + 1]; //in hexa gengs
						break;
					}
				}
			}
		}
	}

	if (!isFound) {
		parentIndex = 0x00;
	}

	return parentIndex;
}

char* searchName(char parentIndex) {
	char files[1024];
	char name[14];
	
	int isFound = 0;
	int isNameMatch, k, s, j, idxName;
	int h, l;

	interrupt(0x21, 0x2, files, 0x101, 0);
	idxName = parentIndex + 2;
	while (files[idxName] != '\0') {
		name[idxName - 2] = files[idxName];
	}

	return name;
}