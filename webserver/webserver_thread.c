#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <pthread.h>


#define LOG 100
#define ERROR 200


#define CODE200 200
#define CODE404 404


#define PHRASE200 "OK"
#define PHRASE404 "FILE NOT FOUND"

char documentRoot[] = "/Users/yunhongmin/Programming/socket-programming/webserver/hdocs";

void* do_web(void *);
void web_log(int, char[], char[], int);

pthread_t tid;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int log_fd;

int main(int argc, char *argv[]) {
	struct sockaddr_in s_addr, c_addr;
	int s_sock, c_sock;
	int len, len_out;
	unsigned short port;
	int status;
	struct rlimit resourceLimit;
	int i, res;

	if (argc != 2) {
		printf("usage: web server port number required");
		return -1;
	}

	//if (fork() != 0)
	//	return 0;  // parent return to shell -> daemon process

	/*
	resourceLimit.rlim_max = 0;
	status = getrlimit(RLIMIT_NOFILE, &resourceLimit);  

	// close all file descriptors from parent
	for (int i = 0; i < resourceLimit.rlim_max; i++) {
		close(i);
	}
	*/

	web_log(LOG, "STATUS", "web server start", getpid());

	if ((s_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		web_log(ERROR, "SYSCALL", "web server listen socket open error", s_sock);
	}

	port = atoi(argv[1]);
	if (port > 60000)
		web_log(ERROR, "ERROR", "invalid port number", port);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_port = htons(port);

	if (bind(s_sock, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0)
			web_log(ERROR, "ERROR", "server cannot bind", 0);

	listen(s_sock, 10);

	while (1) {
		len = sizeof(c_addr);
		if ((c_sock = accept(s_sock, (struct sockaddr *) &c_addr, &len)) < 0)
			web_log(ERROR, "ERROR", "server accept error", 0);

		res = pthread_create(&tid, NULL, do_web, (void *) c_sock);
	}
}

void* do_web(void *arg) {
	int c_sock = (int) arg;

	char sndBuf[BUFSIZ+1], rcvBuf[BUFSIZ+1];
	char uri[100], c_type[20];
	int len;

	int len_out;
	int n, i;
	char *p;
	char method[10], f_name[20];
	char phrase[20] = "OK";

	int code = 200;
	int fd;

	char file_name[20];
	char ext[20];

	struct {
		char *ext;
		char *filetype;
	} extensions [] = {
		{"jpg", "image/jpeg"},
		{"png", "image/png"},
		{"jpeg", "image/jpeg"},
		{"zip", "application/zip"},
		{"htm", "text/html"},
		{"html", "text/html"},
		{0, 0}
	};

	memset(rcvBuf, 0, sizeof(rcvBuf));
	if ((n = read(c_sock, rcvBuf, BUFSIZ)) <= 0)
		web_log(ERROR, "ERROR", "can not receive data from web browser", n);

	web_log(LOG, "REQUEST", rcvBuf, n);

	p = strtok(rcvBuf, " ");
	if (strcmp(p, "GET") && strcmp(p, "get")) {
		web_log(ERROR, "ERROR", "only get method support", 0);
	}

	p = strtok(NULL, " ");
	if (!strcmp(p, "/"))
		sprintf(uri, "%s/index.html", documentRoot);
	else
		sprintf(uri, "%s%s", documentRoot, p);

	strcpy(c_type, "text/plain");
	for (i=0; extensions[i].ext != 0; i++) {
		len = strlen(extensions[i].ext);
		if (!strncmp(uri+strlen(uri)-len, extensions[i].ext, len)) {
			strcpy(c_type, extensions[i].filetype);
			break;
		}
	}

	if ((fd = open(uri, O_RDONLY)) < 0) {
		code = CODE404;
		strcpy(phrase, PHRASE404);
	}

	p = strtok(NULL, "\r\n ");

	sprintf(sndBuf, "HTTP/1.0 %d %s\r\n", code, phrase);
	n = write(c_sock, sndBuf, strlen(sndBuf));
	web_log(LOG, "RESPONSE", sndBuf, getpid());

	sprintf(sndBuf, "content-type: %s\r\n\r\n", c_type);
	n = write(c_sock, sndBuf, strlen(sndBuf));

	web_log(LOG, "RESPONSE", sndBuf, getpid());

	if (fd >= 0) {
		while((n=read(fd, rcvBuf, BUFSIZ)) > 0) {
			write(c_sock, rcvBuf, n);
		}
	}

	close(c_sock);
}

void web_log(int type, char s1[], char s2[], int n) {
	int log_fd;
	char buf[BUFSIZ];

	if (type == LOG) {
		sprintf(buf, "STATUS %s %s %d\n", s1, s2, n);
	} else if (type == ERROR) {
		sprintf(buf, "ERROR %s %s %d\n", s1, s2, n);
	}
	
	pthread_mutex_lock(&mutex);
	if ((log_fd = open("web.log", O_CREAT|O_WRONLY|O_APPEND, 0644)) >= 0) {
		write(log_fd, buf, strlen(buf));
		close(log_fd);
	}
	pthread_mutex_unlock(&mutex);

	if (type == ERROR) exit(-1);
}
