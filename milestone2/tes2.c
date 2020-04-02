void main() {
	int isSuccess;
	interrupt(0x21, 0x00, "Kamu masuk file tes2!\r\n", 0, 0);
	interrupt(0x21, 6, "shell", 0x2000, &isSuccess);
}
