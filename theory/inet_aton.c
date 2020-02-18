#include <stdio.h>
#include <arpa/inet.h>

int main() {
	struct in_addr inp;
	const char *ipAddr = "203.249.39.3";
	char *addr;

	inet_aton(ipAddr, &inp);
	printf("ip (dotted decimal) [%s] -> ip (binary) [0x%x]\n", ipAddr, ntohl(inp.s_addr));
	
	addr = inet_ntoa(inp);
	printf("ip (binary) [0x%x] -> ip (dotted decimal) [%s]\n", htonl(inp.s_addr), addr);
}
