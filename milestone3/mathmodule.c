#include "mathmodule.h"

int mod(int x, int y) { 
    while (x>=y) {
        x-=y;
    }return x;
}

int div (int x, int y) {
	int ratio = 0;
	while(ratio*y <= x) {
		ratio += 1;
	}return(ratio-1);
}