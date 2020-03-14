#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 9000

void* do_echo(void* data);

int main() {
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	pthread_t t_id;
	int len;
	
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
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
		printf("connected \n");
		pthread_create(&t_id, NULL, do_echo, (void *)&c_socket);
	}
}

void * do_echo(void* data) {
	int n;
	char rcvBuffer[BUFSIZ];
	int c_socket = *((int *) data);

	while ((n=read(c_socket, rcvBuffer, sizeof(rcvBuffer))) != 0) {
		rcvBuffer[n] = '\0';
		printf("%s", rcvBuffer);
		write(c_socket, rcvBuffer, n);
	}
	close(c_socket);
	printf("close connection\n");
}

