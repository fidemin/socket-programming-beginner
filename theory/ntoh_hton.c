#include <stdio.h>

main() {
	short hostData = 0x1234;
	short netData;
	char *ch;

	printf("original data: 0x%x\n");

	ch = (char*) &hostData;

	printf("host data: 0x%x%x\n", ch[0], ch[1]);
	netData = htons(hostData);

	ch = (char*) &netData;
	printf("network data: 0x%x%x\n", ch[0], ch[1]);
}
