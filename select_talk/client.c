#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAX 512
#define PORT 9000

char quit[] = "exit";

int main(int argc) {

	int connSock;
	struct sockaddr_in s_addr, c_addr;
	int len, i, n;
	char rbuf[BUFSIZ], sbuf[BUFSIZ];
	int maxfd;

	fd_set read_fds;
	connSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);
	


	if (connect(connSock, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1) {
		printf("Can not conenct\n");
		return -1;
	}

	while(1) {
		maxfd = connSock + 1;
		FD_ZERO(&read_fds);
		FD_SET(0, &read_fds);
		FD_SET(connSock, &read_fds);

		if (select(maxfd, &read_fds, NULL, NULL, NULL) < 0) {
			printf("select error\n");
			exit(-1);
		}

		printf("server waiting--\n");
		if (FD_ISSET(0, &read_fds)) {
			if ((n = read(0, sbuf, BUFSIZ)) > 0) {
				if (write(connSock, sbuf, n) != n) {
					printf("talk server fail in sending\n");
				}
				if (strncmp(sbuf, quit, 4) == 0) {
					break;
				}
			}
		}

		if (FD_ISSET(connSock, &read_fds)) {
			if ((n=read(connSock, rbuf, sizeof(rbuf))) != 0) {
				rbuf[n] = '\0';
				printf("receive %s", rbuf);
			} else {
				printf("conn closed from server\n");
				break;
			}

			if (strncmp(rbuf, quit, 4) == 0) {
				break;
			}
		}
	}
	close(connSock);
	return 0;
}
