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

    // Checks the number of processes allowed.
    if (numtasks != 2) {
        printf("Wrong number of processes. Only 2 allowed!\n");
        MPI_Finalize();
        return 0;
    }

    // How many numbers will be sent.
    int send_numbers = 10;
    int *nums = (int *) malloc(send_numbers * sizeof(int));
    int *tags = (int *) malloc(send_numbers * sizeof(int));

    if (rank == 0) {
        for (int i = 0; i < send_numbers; i++) {
            nums[i] = rand() % 25;
            tags[i] = rand() % 12;
            MPI_Send(&nums[i], 1, MPI_INT, 1, tags[i], MPI_COMM_WORLD);
        }
    } else {

        for (int i = 0; i < send_numbers; i++) {
            MPI_Status status;
            MPI_Recv(&nums[i], 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("Received %d with tag %d \n", nums[i], status.MPI_TAG);
        }
    }

    MPI_Finalize();

}

