#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

void sigAlarm(int);


int main() {
	int sd;
	struct sockaddr_in s_addr;
	char sndBuffer[BUFSIZ];
	int n, n_send;
	int addr_len;

	int status;
	struct sigaction act;

	// handling alarm signal
	act.sa_handler = sigAlarm;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	status = sigaction(SIGALRM, &act, 0);

	sd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&s_addr, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	s_addr.sin_port = htons(9000);

	while (1) {
		fprintf(stderr, "waiting\n");

		if ((n = read(0, sndBuffer, BUFSIZ)) > 0) {
			sndBuffer[n] = '\0';
			if (!strcmp(sndBuffer, "quit\n")) break;
			
			printf("original Data: %s", sndBuffer);
			
			if ((n_send = sendto(sd, sndBuffer, strlen(sndBuffer), 0, (struct socketaddr* ) &s_addr, sizeof(s_addr))) < 0) {
				fprintf(stderr, "recvfrom() error");
				exit(-3);
			}

			alarm(2);  // set alarm after 2 seconds

			addr_len = sizeof(s_addr);
			fprintf(stderr, "socket recvfrom enter\n");
			if ((n = recvfrom(sd, sndBuffer, sizeof(sndBuffer), 0, NULL, NULL)) < 0) {
				if (errno == EINTR)
					fprintf(stderr, "socket timeout\n");
				else
					fprintf(stderr, "recvfrom error\n");
			} else {
				alarm(0);  // cancel alarm
				sndBuffer[n] = '\0';
				printf("echoed Data: %s", sndBuffer);
			}
		}
	
	}
	close(sd);
}

void sigAlarm(int signo) {
	fprintf(stderr, "alarm\n");
}
