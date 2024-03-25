#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define ROOT 0

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int rand_num;

    srand(42);
    rand_num = rand() % 100;

    MPI_Bcast(&rand_num, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    // Root process generates a random number.
    // Broadcasts to all processes.

    printf("Process [%d], after broadcast %d.\n", rank, rand_num);

    MPI_Finalize();

}

