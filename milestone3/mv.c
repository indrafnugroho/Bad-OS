#include "textmodule.h"
#include "fileIOmodule.h"
#include "folderIOmodule.h"

void mv(char* cmd, int* idxDir) {
	char directory[14];
	char dirDipindah[14];
	char files[1024];
	int count, val, nomorPindah, lanjot, var, initDir, dirTujuan,i;
	int panjang = 512;
	count = 0;
	lanjot = 1;
	var = 0;
	initDir = *(idxDir);
	dirTujuan = *(idxDir);
	
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
			if(cmd[i] == 32 && cmd[i] == 0) {
				dirDipindah[count] = cmd[i];
				++count;
			} else if (cmd[i] == 64) {
				nomorPindah = searchPath(dirDipindah, *idxDir);
				if(nomorPindah == 64) {
					interrupt(0x21, 0, "Gaiso mindah iki bro! : \0",0,0);
					interrupt(0x21, 0, dirDipindah, 0, 0);
					interrupt(0x21, 0, "\r\n\0", 0, 0);
					lanjot = 0;
				} else {
					count = 0;
					var = 1;
				}
			}
			if(cmd[i+1] == 32 || cmd[i+2] == 32){
				interrupt(0x21, 0, "Out of bounds!\r\n\0",0,0);
				lanjot = 0;
			}
		}
		else if(var == 1) {
			//cd di tempat tujuan
			if(cmd[i] == '/') {
				//isi array
				directory[count] = cmd[i];
				++count;
			} else if(cmd[i] == 32 || cmd[i] == '/') {
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
}