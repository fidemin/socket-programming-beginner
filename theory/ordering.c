#include <stdio.h>

int main() {
	short data;
	char *ch;

	data = 0x1234;
	ch = (char*) &data;

	printf("original data: 0x%x\n", data);

	if (ch[0] == 0x12 && ch[1] == 0x34)
		printf("Host Order Type: big-endian -> ");
	else if (ch[0] == 0x34 && ch[1] == 0x12)
		printf("Host Order Type: little-endian -> ");

	printf("0x%x%x\n", ch[0], ch[1]);
}
