#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/*
    schelet pentru exercitiul 5
*/
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

int* arr;
int array_size;
int num_threads = 4;

void *f(void *arg) {
  	long id = *(long*) arg;
	int start = id * (double) array_size / num_threads;
    int end = MIN((id + 1) * (double) array_size / num_threads, array_size);
    for (int i = start; i < end; i++) {
        arr[i] += 100;
    }
  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Specificati dimensiunea array-ului\n");
        exit(-1);
    }

    array_size = atoi(argv[1]);

    arr = malloc(array_size * sizeof(int));
    for (int i = 0; i < array_size; i++) {
        arr[i] = i;
    }

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

    pthread_t threads[num_threads];
  	long id;
  	void *status;
  	long arguments[num_threads];
  	int r;

  	for (id = 0; id < num_threads; id++) {
        arguments[id] = id;
        r = pthread_create(&threads[id], NULL, f, &arguments[id]);

        if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
    }


    for (id = 0; id < num_threads; id++) {
        pthread_join(threads[id], &status);

        if (r) {
	  		printf("Eroare la asteptarea thread-ului %ld\n", id);
	  		exit(-1);
		}
    }

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

  	pthread_exit(NULL);
}
