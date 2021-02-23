#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


void *f(void *arg) {
  	long id = *(long*) arg;
  	printf("Hello World din thread-ul %ld!\n", id);
  	pthread_exit(NULL);
}

void *g(void *arg) {
  	long id = *(long*) arg;
  	printf("Hello World din thread-ul %ld!\n", id);
  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t threads[2];
  	long arguments[2];

	arguments[0] = 0;
	pthread_create(&threads[0], NULL, f, &arguments[0]);

	arguments[1] = 1;
	pthread_create(&threads[1], NULL, g, &arguments[1]);

	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

  	pthread_exit(NULL);
}
