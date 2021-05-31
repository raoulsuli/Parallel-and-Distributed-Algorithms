#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
pthread_barrier_t barrier;

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;

int P;

// structura pentru un numar complex
typedef struct _complex {
	double a;
	double b;
} complex;

// structura pentru parametrii unei rulari
typedef struct _params {
	int is_julia, iterations;
	double x_min, x_max, y_min, y_max, resolution;
	complex c_julia;
} params;

typedef struct {

	params parJ;
	int **resultJ;
	int widthJ;
	int heightJ;
    
	params parM;
	int **resultM;
	int widthM;
	int heightM;

	int id;
} arg_struct;

// citeste argumentele programului
void get_args(int argc, char **argv)
{
	if (argc < 6) {
		printf("Numar insuficient de parametri:\n\t"
				"./tema1_par fisier_intrare_julia fisier_iesire_julia "
				"fisier_intrare_mandelbrot fisier_iesire_mandelbrot P\n");
		exit(1);
	}

	in_filename_julia = argv[1];
	out_filename_julia = argv[2];
	in_filename_mandelbrot = argv[3];
	out_filename_mandelbrot = argv[4];
	P = atoi(argv[5]);
}

// citeste fisierul de intrare
void read_input_file(char *in_filename, params* par)
{
	FILE *file = fopen(in_filename, "r");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de intrare!\n");
		exit(1);
	}

	fscanf(file, "%d", &par->is_julia);
	fscanf(file, "%lf %lf %lf %lf",
			&par->x_min, &par->x_max, &par->y_min, &par->y_max);
	fscanf(file, "%lf", &par->resolution);
	fscanf(file, "%d", &par->iterations);

	if (par->is_julia) {
		fscanf(file, "%lf %lf", &par->c_julia.a, &par->c_julia.b);
	}

	fclose(file);
}

// scrie rezultatul in fisierul de iesire
void write_output_file(char *out_filename, int **result, int width, int height)
{
	int i, j;

	FILE *file = fopen(out_filename, "w");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de iesire!\n");
		return;
	}

	fprintf(file, "P2\n%d %d\n255\n", width, height);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

// aloca memorie pentru rezultat
int **allocate_memory(int width, int height)
{
	int **result;
	int i;

	result = malloc(height * sizeof(int*));
	if (result == NULL) {
		printf("Eroare la malloc!\n");
		exit(1);
	}

	for (i = 0; i < height; i++) {
		result[i] = malloc(width * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	return result;
}

// elibereaza memoria alocata
void free_memory(int **result, int height)
{
	int i;

	for (i = 0; i < height; i++) {
		free(result[i]);
	}
	free(result);
}

// ruleaza algoritmul Julia
void run_julia(params *par, int **result, int width, int height, int id)
{
    int start = id * (double) height / P;
    int end = MIN((id + 1) * (double) height / P, height);
    //calculam intervalul corespunzator

	int w, h, i;

	for (w = 0; w < width; w++) {
		for (h = start; h < end; h++) {
			int step = 0;
			complex z = { .a = w * par->resolution + par->x_min,
							.b = h * par->resolution + par->y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + par->c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + par->c_julia.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

    pthread_barrier_wait(&barrier);

    start = id * (double) height / (2 * P);
    end = MIN((id + 1) * (double) height / (2 * P), height / 2);

    // transforma rezultatul din coordonate matematice in coordonate ecran
    for (i = start; i < end; i++) {
    	int *aux = result[i];
	    result[i] = result[height - i - 1];
	    result[height - i - 1] = aux;
    }
}

// ruleaza algoritmul Mandelbrot
void run_mandelbrot(params *par, int **result, int width, int height, int id)
{

    int start = id * (double) height / P;
    int end = MIN((id + 1) * (double) height / P, height);

	int w, h, i;
    
    // paralelizam calculele in functie de inaltimea imaginii 

	for (w = 0; w < width; w++) {
		for (h = start; h < end; h++) {
			complex c = { .a = w * par->resolution + par->x_min,
							.b = h * par->resolution + par->y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

    //asteptam ca toate prelucrarile sa se fi terminat dupa care transformam coordonatele

    pthread_barrier_wait(&barrier);
    
    //limita superioara va fi height / 2 in acest caz
    //vom imparti la height / 2 * nr_thread-uri pentru a fi distribuit uniform pe thread-uri
    start = id * (double) height / (2 * P);
    end = MIN((id + 1) * (double) height / (2 * P), height / 2);

	// transforma rezultatul din coordonate matematice in coordonate ecran
    for (i = start; i < end; i++) {
	    int *aux = result[i];
	    result[i] = result[height - i - 1];
	    result[height - i - 1] = aux;
    }
}
    
void *function(void *arg) {
	arg_struct args = *(arg_struct *)arg;

	run_julia(&args.parJ, args.resultJ, args.widthJ, args.heightJ, args.id);

	pthread_barrier_wait(&barrier); // asteptam terminarea calcului matricei pentru a scrie in fisierul de out

    //vrem sa scriem output-ul o singura data (cu un singur thread)

    if (args.id == 0) {
        write_output_file(out_filename_julia, args.resultJ, args.widthJ, args. heightJ);
    }

	run_mandelbrot(&args.parM, args.resultM, args.widthM, args.heightM, args.id);
	
    pthread_barrier_wait(&barrier);
    
    if (args.id == 0) {
	    write_output_file(out_filename_mandelbrot, args.resultM, args.widthM, args.heightM);
    }

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{   
	get_args(argc, argv);
	pthread_t threads[P];
	
    arg_struct *args = malloc(P * sizeof(arg_struct)); // structura in care salvam datele problemei pentru a le trimite in thread

	pthread_barrier_init(&barrier, NULL, P);

	params parJ;
	int widthJ, heightJ;
	int **resultJ;
	params parM;
	int widthM, heightM;
	int **resultM;       // cate un set de variabile pentru fiecare multime

	// se citesc argumentele programului
	
	read_input_file(in_filename_julia, &parJ);
	widthJ = (parJ.x_max - parJ.x_min) / parJ.resolution;
	heightJ = (parJ.y_max - parJ.y_min) / parJ.resolution;
	resultJ = allocate_memory(widthJ, heightJ);

	read_input_file(in_filename_mandelbrot, &parM);
	widthM = (parM.x_max - parM.x_min) / parM.resolution;
	heightM = (parM.y_max - parM.y_min) / parM.resolution;
	resultM = allocate_memory(widthM, heightM);

    //asociem fiecarui thread_id datele corespunzatoare
	for (int id = 0; id < P; id++) {
		args[id].parJ = parJ;
		args[id].parM = parM;
		args[id].resultJ = resultJ;
		args[id].resultM = resultM;
		args[id].widthJ = widthJ;
		args[id].widthM = widthM;
		args[id].heightJ = heightJ;
		args[id].heightM = heightM;
	}

	for (int id = 0; id < P; id++) {
		args[id].id = id;
		pthread_create(&threads[id], NULL, function, &args[id]);
	}

	for (int id = 0; id < P; id++) {
		pthread_join(threads[id], NULL);
	}
	
	free_memory(resultJ, heightJ);
	free_memory(resultM, heightM);
	
	free(args);
	pthread_barrier_destroy(&barrier);
  	return 0;
}
