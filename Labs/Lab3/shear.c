#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "pthread_barrier_mac.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

int L; // latura matricei
int N; // numarul de elemente de sortat
int P;
int *v;
int *vQSort;
int **M;

pthread_barrier_t barrier;

void compare_vectors(int *a, int *b) {
	int i;

	for (i = 0; i < N; i++) {
		if (a[i] != b[i]) {
			printf("Sortare incorecta\n");
			return;
		}
	}

	printf("Sortare corecta\n");
}

void display_vector(int *v) {
	int i;
	int display_width = 2 + log10(N);

	for (i = 0; i < N; i++) {
		printf("%*i", display_width, v[i]);
	}

	printf("\n");
}

void display_matrix(int **M) {
	int i, j;
	int display_width = 2 + log10(N);

	for (i = 0; i < L; i++) {
		for (j = 0; j < L; j++) {
			printf("%*i", display_width, M[i][j]);
		}
		printf("\n");
	}
}

void copy_matrix_in_vector(int *v, int **M) {
	int i, j;
	for (i = 0; i < L; i++) {
		if (i % 2 == 0) {
			for (j = 0; j < L; j++) {
				v[i * L + j] = M[i][j];
			}
		} else {
			for (j = L; j > 0; j--) {
				v[i * L + (L - j)] = M[i][j-1];
			}
		}
	}
}

int cmp(const void *a, const void *b) {
	int A = *(int*)a;
	int B = *(int*)b;
	return A - B;
}

int cmpdesc(const void *a, const void *b) {
	int A = *(int*)a;
	int B = *(int*)b;
	return B - A;
}

void get_args(int argc, char **argv)
{
	if(argc < 3) {
		printf("Numar insuficient de parametri: ./shear L P (L = latura matricei)\n");
		exit(1);
	}

	L = atoi(argv[1]);
	N = L * L;
	P = atoi(argv[2]);
}

void init()
{
	int i, j;
	v = malloc(sizeof(int) * N);
	vQSort = malloc(sizeof(int) * N);
	M = malloc(sizeof(int*) * L);

	if (v == NULL || vQSort == NULL || M == NULL) {
		printf("Eroare la malloc!");
		exit(1);
	}

	for (i = 0; i < L; i++) {
		M[i] = malloc(sizeof(int) * L);
	}

	srand(42);

	for (i = 0; i < L; i++) {
		for (j = 0; j < L; j++) {
			M[i][j] = rand() % N;
		}
	}
}

void print()
{
	printf("M:\n");
	display_matrix(M);

	copy_matrix_in_vector(v, M);
	printf("v:\n");
	display_vector(v);
	printf("vQSort:\n");
	display_vector(vQSort);
	compare_vectors(v, vQSort);	
}

void *thread_function(void *arg)
{	
	int aux[L];
	double ratio = (double) L/P;
	int thread_id = *(int *)arg;
	
	int start = thread_id * ratio;
	int end = MIN((thread_id + 1) * ratio, L);
	int start_even = (start % 2 == 0) ? start : start + 1;
	int start_odd = (start % 2 == 1) ? start : start + 1;

	for (int k = 0; k < log(L * L) + 1; k++) {
		for (int i = start_even; i < end; i += 2) {
			qsort(M[i], L, sizeof(int), cmp);
		}
		pthread_barrier_wait(&barrier);
		for (int i = start_odd; i < end; i += 2) {
			qsort(M[i], L, sizeof(int), cmpdesc);
		}
		pthread_barrier_wait(&barrier);

		for (int i = start; i < end; i++) {
			for (int j = 0; j < L; j++) {
				aux[j] = M[j][i];
			}

			qsort(aux, L, sizeof(int), cmp);

			for (int j = 0; j < L; j++) {
				M[j][i] = aux[j];
			}
		}
		pthread_barrier_wait(&barrier);
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	get_args(argc, argv);
	init();

	int i;
	pthread_t tid[P];
	int thread_id[P];
	pthread_barrier_init(&barrier, NULL, P);

	pthread_barrier_init(&barrier, NULL, P);

	// se sorteaza etalonul
	copy_matrix_in_vector(vQSort, M);
	qsort(vQSort, N, sizeof(int), cmp);

	// se creeaza thread-urile
	for (i = 0; i < P; i++) {
		thread_id[i] = i;
		pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);
	}

	// se asteapta thread-urile
	for (i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	// se afiseaza matricea
	// se afiseaza vectorul etalon
	// se afiseaza matricea curenta sub forma de vector
	// se compara cele doua
	print();

	free(v);
	free(vQSort);
	for (i = 0; i < L; i++) {
		free(M[i]);
	}
	free(M);
	pthread_barrier_destroy(&barrier);

	return 0;
}