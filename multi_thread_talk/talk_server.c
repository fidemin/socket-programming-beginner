#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>

void* read_keyboard(void*);
void* read_socket(void*);

char quit[] = "exit";

pthread_t tid[2];

int main(int argc, char *argv[]) {
	int listenSock, connSock;
	struct sockaddr_in client_addr, server_addr;
	int len;
	int status;
	pthread_t t_id;

	if (argc < 2) {
		printf("Usage: s.out {port_num}\n");
		return -1;
	}

	if ((listenSock=socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Server: Can't open listen socket\n");
		return -1;
	}

	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(listenSock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		printf("Talk Server can't bind \n");
		return -1;
	}

	listen(listenSock, 1);

	len = sizeof(client_addr);

	if ((connSock=accept(listenSock, (struct sockaddr*) &client_addr, &len)) < 0) {
		printf("Talk Server failed in accepting\n");
		return -1;
	}
	
	printf("Talk Server accept new request\n");

	pthread_create(&tid[0], NULL, read_keyboard, (void*) connSock);
	pthread_create(&tid[1], NULL, read_socket, (void*) connSock);

	pthread_join(tid[0], (void**) &status);
	pthread_join(tid[1], (void**) &status);

	close(listenSock);
	close(connSock);
}


void *read_keyboard(void* data) {
	int connSock = (int) data;
	int n;
	char sbuf[BUFSIZ];

	while ((n = read(0, sbuf, BUFSIZ)) > 0) {
		if (write(connSock, sbuf, n) != n) {
			pthread_kill(tid[1], SIGINT);
			printf("talk server fail in sending\n");
		}

		if (strncmp(sbuf, quit, 4) == 0) {
			break;
		}

	}
}


void *read_socket(void* data) {
	int connSock = (int) data;
	int n;
	char rbuf[BUFSIZ];

	while (1) {
		if ((n = read(connSock, rbuf, BUFSIZ)) > 0) {
			rbuf[n] = '\0';
			printf("%s", rbuf);

			if (strncmp(rbuf, quit, 4) == 0) {
				pthread_kill(tid[0], SIGINT);
				break;
			}
		} 
	}
}

