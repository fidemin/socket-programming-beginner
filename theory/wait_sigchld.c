#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

void do_child(void);
void do_parent(void);

void sighandler(int n) {
	int status, chld_pid;
	sigset_t newset, oldset;

	chld_pid = wait(&status);
	printf("signal NO [%d]\n", n);
	printf("child PID [%d]\n", chld_pid);

	if (WIFEXITED(status)) {
		printf("normal termination, exit status [%d]\n", WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
		printf("abnormal termination, signal number [%d]\n", WTERMSIG(status));
	} else if (WIFSTOPPED(status)) {
		printf("child stopped, signal number [%d]\n", WSTOPSIG(status));
	}

	exit(1);
}

int main() {
	int pid;
	struct sigaction sigset, oldset;

	sigset.sa_handler = sighandler;
	sigset.sa_flags = 0;
	sigemptyset(&sigset.sa_mask);
	sigaction(SIGCHLD, &sigset, &oldset);

	if ((pid=fork()) == 0) {
		do_child();
	} else if (pid > 0) {
		do_parent();
	}
}

void do_child() {
	//exit(101);
	abort();
}

void do_parent() {
	sleep(10);
}
