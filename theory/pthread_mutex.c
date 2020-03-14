#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int ncount;
// initializer mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * do_sum1(void *data) {
	int i;
	int n = *((int *) data);;

	for (i = 0; i < n; i++) {
		pthread_mutex_lock(&mutex);
		printf("sum1: %d\n", ncount);
		ncount++;
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
}

void * do_sum2(void *data) {
	int i;
	int n = *((int *) data);;

	for (i = 0; i < n; i++) {
		pthread_mutex_lock(&mutex);
		printf("sum2: %d\n", ncount);
		ncount++;
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
}

int main() {
	int thr_id;
	pthread_t p_thread[2];

	int status;

	ncount = 1;

	int n = 9;
	thr_id = pthread_create(&p_thread[0], NULL, do_sum1, (void *)&n);
	sleep(1);
	n = 10;
	thr_id = pthread_create(&p_thread[1], NULL, do_sum2, (void *)&n);
	
	pthread_join(p_thread[0], (void**) &status);
	pthread_join(p_thread[1], (void**) &status);

	// destroy mutex
	pthread_mutex_destroy(&mutex);
	printf("this is end");

	return 0;
}


