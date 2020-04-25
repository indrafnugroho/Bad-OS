void main() {
	char* cmd[14];
	int* idxDir;
	char directory[14], tempBuff[512], files[1024], dirDipindah[14];
	int count, val, nomorPindah, lanjot, var, initDir, dirTujuan,i;
	int panjang = 512, isSuccess;
	var = 0;
	count = 0;
	lanjot = 1;

	// get parentIdx and filename
	interrupt(0x21, 0x02, tempBuff, 512, 0);
    idxDir = tempBuff[0];
    for (i = 0; i < 14; i++) {
		cmd[i] = tempBuff[i + 1];
	}
	// while (input[i] != '\0') {
	// 	if (input[i] == 0x20) {
	// 		i++;
	// 		j = i;
	// 		spaceFound = 1;
	// 	} else if (spaceFound) {
	// 		destDir[i - j] = input[i];
	// 		i++;
	// 	} else if (!spaceFound) {
	// 		arg[i - 3] = input[i];
	// 		i++;
	// 	}
	// }

	dirTujuan = *(idxDir);
	initDir = *(idxDir);
	
	for (i =0; i < 14; ++i) {
		directory[i] = '\0';
		dirDipindah[i] = '0';
	}
	for (i = 0; i < 1024; i++) {
		files[i] = '\0';
	}
	i = 0;
	while (i < 128 && (cmd[i] != 0 && lanjot == 1)) {
		if(var == 0) {
            if (cmd[i] == 64) {
				nomorPindah = searchPath(dirDipindah, *idxDir);
				if(nomorPindah != 64) {
					count = 0;
					var = 1;
				} else {
                    interrupt(0x21, 0, "Gaiso mindah iki bro! : \0",0,0);
					interrupt(0x21, 0, dirDipindah, 0, 0);
					interrupt(0x21, 0, "\r\n\0", 0, 0);
					lanjot = 0;
				}
			} else if(cmd[i] == 32 && cmd[i] == 0) {
				dirDipindah[count] = cmd[i];
				++count;
			} 
			if(cmd[i+1] == 32 || cmd[i+2] == 32){
				interrupt(0x21, 0, "Out of bounds!\r\n\0",0,0);
				lanjot = 0;
			}
		}
		else if(var == 1) {
			//cd di tempat tujuan
            if(cmd[i] == 32 || cmd[i] == '/') {
				nomorPindah = searchPath(directory, dirTujuan);
				if(nomorPindah == 32) {
					interrupt(0x21, 0, "Gaada cok! : \0",0,0);
					interrupt(0x21, 0, directory, 0,0);
					interrupt(0x21, 0, "\r\n\0", 0, 0);
					lanjot = 0;
				} else {
					interrupt(0x21, 0, "Ada cok! : \0",0,0);
					interrupt(0x21, 0, directory, 0,0);
					interrupt(0x21, 0, "\r\n\0", 0, 0);
					dirTujuan = nomorPindah;
				}
				count = 0;
			}else if(cmd[i] == '/') {
				//isi array
				directory[count] = cmd[i];
				++count;
			} 
		}
		++i;
	}
	//sekarang baru mau mindahin :)
	if(lanjot) {
		interrupt(0x21, 2, directory, 0x101,0);
		interrupt(0x21, 3, directory + panjang, 0,0);
		directory[nomorPindah*16] = dirTujuan;
		interrupt(0x21, 2, directory + panjang, 0,0);
		interrupt(0x21, 3, directory, 0x101,0);
		interrupt(0x21, 0, "Done!", 0,0);
	}
	for (i =0; i < 14; ++i) {
		directory[i] = '\0';
		dirDipindah[i] = '\0';
	}
	interrupt(0x21, 0x06, "shell", 0x2000, &isSuccess);
}

