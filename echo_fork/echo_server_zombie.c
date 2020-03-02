#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 9000

void do_echo(int);

int main() {
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;

	int len;
	int n;
	pid_t pid;

	char rcvBuffer[BUFSIZ];
	
	s_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	if (bind(s_socket, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1) {
		printf("Can not bind\n");
		return -1;
	}

	if (listen(s_socket, 5) == -1) {
		printf("Listen fail\n");
		return -1;
	}

	while(1) {
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);

		if ((pid = fork()) < 0) {
			printf("echo server can not fork()\n");
			return -1;
		} else if (pid > 0) {
			close(c_socket);
			continue;
		} else if (pid == 0) {
			close(s_socket);
			do_echo(c_socket);
		}
	}
}

void do_echo(int c_socket) {
	int n;
	char rcvBuffer[BUFSIZ];

	while ((n = read(c_socket, rcvBuffer, sizeof(rcvBuffer))) != 0) {
		printf("child\n");
		write(c_socket, rcvBuffer, n);
	}
	exit(0);
}

