// mkdir.c

void main () {
    int isSuccess;
	makeDir();
    interrupt(0x21, 0x06, "shell", 0x2000, &isSuccess);
}