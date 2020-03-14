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
	int conn_sock;
	struct sockaddr_in server_addr;
	char *server_host;
	int server_port;
	int status;

	if (argc != 3) {
		printf("Usage: c.out {server_ip} {port_num}\n");
		return -1;
	}

	server_host = argv[1];
	server_port = atoi(argv[2]);


	if ((conn_sock=socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Client: Can't open socket\n");
		return -1;
	}

	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_host);
	server_addr.sin_port = htons(server_port);

	if (connect(conn_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		printf("Client: cannot connect to server \n");
		return -1;
	}
	printf("Client: connect to talk server\n");

	pthread_create(&tid[0], NULL, read_keyboard, (void*) conn_sock);
	pthread_create(&tid[1], NULL, read_socket, (void*) conn_sock);

	pthread_join(tid[0], (void**) &status);
	pthread_join(tid[1], (void**) &status);

	close(conn_sock);
}


void* read_keyboard(void* data) {
	int conn_sock = (int) data;
	int n;
	char sbuf[BUFSIZ];

	while ((n = read(0, sbuf, BUFSIZ)) > 0) {
		if (write(conn_sock, sbuf, n) != n) {
			printf("talk server fail in sending\n");
		}

		if (strncmp(sbuf, quit, 4) == 0) {
			pthread_kill(tid[1], SIGINT);
			break;
		}
	}
}


void* read_socket(void* data) {
	int conn_sock = (int) data;
	
	int n;
	char rbuf[BUFSIZ];

	while (1) {
		if ((n = read(conn_sock, rbuf, BUFSIZ)) > 0) {
			rbuf[n] = '\0';
			printf("%s", rbuf);

			if (strncmp(rbuf, quit, 4) == 0) {
				pthread_kill(tid[0], SIGINT);
				break;
			}
		} 
	}
}

