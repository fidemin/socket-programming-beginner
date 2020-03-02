#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define PORT 9000

void do_echo(int);
void sigHandler(int);

int main() {
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;

	int len;
	int n;
	pid_t pid;
	struct sigaction act;

	act.sa_handler = sigHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	int state = sigaction(SIGCHLD, &act, NULL);

	if (state != 0) {
		printf("signal error\n");
		exit(1);
	}

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
		printf("Accept starts\n");
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);

		// In case of child process is quitted, errno variable has EINTR value and accept function returns directly. -> infinite loop in while...
		if (errno == EINTR)
			continue;
		else
			printf("accept error\n");

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
	printf("new echo starts\n");

	while ((n = read(c_socket, rcvBuffer, sizeof(rcvBuffer))) != 0) {
		printf("received buffer size: %d\n", n);
		printf("socket reading in child\n");
		rcvBuffer[n] = '\0';
		printf("rcvBuffer: %s\n", rcvBuffer);
		write(c_socket, rcvBuffer, n);
		sleep(1);
	}

	printf("quitting do_echo...\n");
	exit(0);
}

void sigHandler(int sig) {
	int pid;
	int status;

	pid = wait(&status);
	printf("pid[%d] terminated with signal number[%d]\n", pid, sig);
}

