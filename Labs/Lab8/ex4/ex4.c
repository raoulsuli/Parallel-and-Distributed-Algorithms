#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROOT 1

int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(hostname, &len);

    int value;

    //Am facut exercitiile intr-o masina virtuala si nu am 4 procesoare alocate.
    // Am modificat root-ul sa fie 1 si am schimbat rand-ul, dar ar trebuie sa functioneze codul si pe alt pc cu 4 procesoare.

    if (rank == ROOT) {
        MPI_Status status;
        // The ROOT process receives an element from any source.
        // Prints the element and the source. HINT: MPI_Status.
        MPI_Recv(&value, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
        printf("Root received %d from %d.\n", value, status.MPI_SOURCE);

    } else {

        // Generate a random number.
        srand(time(NULL));
        value = rand() % ((rank + 2) * 50 + 1);

        printf("Process [%d] send %d.\n", rank, value);

        MPI_Send(&value, 1, MPI_INT, ROOT, 1, MPI_COMM_WORLD);
        // Sends the value to the ROOT process.

    }

    MPI_Finalize();

}

