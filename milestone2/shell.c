int main() {
	char curdir = 0xFF;
	char* input;

	while (1) {
		do {
			if (curdir = 0XFF) {
				interrupt(0x21, 0x00, "Root", 0, 0);
			} else {

			}
			interrupt(0x21, 0x00, ">", 0, 0);
			interrupt(0x21, 0x01, input, 1, 0);
		} while (input == "");

		interrupt(0x21, 0x00, "lewat\r\n", 0, 0);
		interrupt(0x21, 0x00, input, 0, 0);
	}

	return 0;
}

