#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int recv_num;

    // First process starts the circle.

    if (rank == 0) {
        srand(42);
        int token = rand() % 100;
        printf("%d sent to %d, token %d\n", rank, rank + 1, token);
        MPI_Send(&token, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
        MPI_Recv(&recv_num, 1, MPI_INT, numtasks - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%d received from %d, token %d\n", rank, numtasks - 1, recv_num);
    } else if (rank == numtasks - 1) {
        MPI_Recv(&recv_num, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%d received from %d, token %d\n", rank, rank - 1, recv_num);
        recv_num += 2;
        printf("%d sent to %d, token %d\n", rank, 0, recv_num);
        MPI_Send(&recv_num, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&recv_num, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("%d received from %d, token %d\n", rank, rank - 1, recv_num);
        recv_num += 2;
        printf("%d sent to %d, token %d\n", rank, rank + 1, recv_num);
        MPI_Send(&recv_num, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();

}

