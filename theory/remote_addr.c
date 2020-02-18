#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int main() {
	int tcpSd;
	struct sockaddr_in s_addr;
	const char *ipAddr = "203.249.39.3";

	if ((tcpSd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
		exit(-1);
	}

	bzero((char*) &s_addr, sizeof(s_addr));

	s_addr.sin_family = AF_INET;
	inet_aton(ipAddr, &s_addr.sin_addr);
	s_addr.sin_port = htons(7);
	printf("ip (dotted decimal) = %s\n", inet_ntoa(s_addr.sin_addr));
	printf("ip (binary) = %x\n", ntohl(s_addr.sin_addr.s_addr));
	printf("port no = %d\n", ntohs(s_addr.sin_port));

	close(tcpSd);
	return 0;

}
