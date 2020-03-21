#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>


#define PORT 8090
#define MAX 512

int numClient = 0;
int clientSock[MAX];

int getMaxfd(int);

int main() {
	int connSock, listenSock;
	struct sockaddr_in s_addr, c_addr;
	int len, i, n;
	char rcvBuffer[BUFSIZ];
	int maxfd;

	fd_set read_fds;

	listenSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	if (bind(listenSock, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1) {
		printf("Can not Bind\n");
		return -1;
	}

	if (listen(listenSock, 5) == -1) {
		printf("listen Fail\n");
		return -1;
	}

	while (1) {
		maxfd = getMaxfd(listenSock) + 1;
		FD_ZERO(&read_fds);
		FD_SET(listenSock, &read_fds);

		for (i=0; i < numClient; i++)
			FD_SET(clientSock[i], &read_fds);

		// waiting for connection
		if (select(maxfd, &read_fds, NULL, NULL, NULL)  < 0) {
			printf("select error\n");
			exit(-1);
		}

		printf("select waiting---\n");
		if (FD_ISSET(listenSock, &read_fds)) {
			connSock=accept(listenSock, (struct sockaddr *) &c_addr, &len);
			clientSock[numClient++] = connSock;
		}

		for (i=0; i < numClient; i++) {
			if (FD_ISSET(clientSock[i], &read_fds)) {
				if ((n=read(clientSock[i], rcvBuffer, sizeof(rcvBuffer))) != 0) {
					rcvBuffer[n] = '\0'; // without \0, printf with rcvBuffer not working well.
					printf("receive - %s", rcvBuffer);
					write(clientSock[i], rcvBuffer, n);
					printf("send - %s", rcvBuffer);
				} else { // n = 0 means client try to close connection
					printf("close connection from client\n");
					close(clientSock[i]);
					if (i != numClient-1)
						clientSock[i] = clientSock[numClient-1];
					numClient--;
					continue;
				}
			}
		}
	}

	return 0;
}


int getMaxfd(int n) {
	int max = n;
	int i;

	for (i = 0; i < numClient; i++) {
		if (clientSock[i] > max) {
			max = clientSock[i];
		}
	}
	return max;
}
