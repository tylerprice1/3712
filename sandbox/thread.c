#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_t thread;
pthread_mutex_t mutex;
int x = 0;
const int COUNT = 10;

void *
callback(void * arg);

int
main(void) {
	int i;
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&thread, NULL, &callback, NULL);

	pthread_mutex_lock( &mutex );
	for (i = 0; i < COUNT; i++) {
		x++;
		printf("main:     x = %d\n", x);
	}
	pthread_mutex_unlock( &mutex );

	system("ps");
}

void *
callback(void * arg) {
	int i;
	for (i = 0; i < COUNT; i++) {
		pthread_mutex_lock( &mutex );
		x++;
		printf("callback: x = %d\n", x);
		pthread_mutex_unlock( &mutex );
	}
	return NULL;
}
