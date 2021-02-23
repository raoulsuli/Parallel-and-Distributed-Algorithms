#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define CONVERGENCE_COEF 100

/**
 * Run: mpirun -np 12 ./a.out
 */

static int num_neigh;
static int *neigh;

void read_neighbours(int rank) {
    FILE *fp;
    char file_name[15];
    sprintf(file_name, "./files/%d.in", rank);

    fp = fopen(file_name, "r");
	fscanf(fp, "%d", &num_neigh);

	neigh = malloc(sizeof(int) * num_neigh);

	for (size_t i = 0; i < num_neigh; i++)
		fscanf(fp, "%d", &neigh[i]);
}

int* get_dst(int rank, int numProcs, int leader) {
	MPI_Status status;
	MPI_Request request;

	/* Vectori de parinti */
	int *v = malloc(sizeof(int) * numProcs);
	int *vRecv = malloc(sizeof(int) * numProcs);

	memset(v, -1, sizeof(int) * numProcs);
	memset(vRecv, -1, sizeof(int) * numProcs);
	
	if (rank == leader)
		v[rank] = -1;
	else {
		/* Daca procesul curent nu este liderul, inseamna ca va astepta un mesaj de la un parinte */
		MPI_Recv(vRecv, numProcs, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		v[rank] = status.MPI_SOURCE;
	}

	for (int i = 0; i < num_neigh; i++) {
		if (v[rank] == neigh[i]) continue;
		MPI_Send(v, numProcs, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
	}

	for(int i = 0; i < num_neigh; i++) {
		if(neigh[i] == v[rank]) continue;
		MPI_Recv(vRecv, numProcs, MPI_INT, neigh[i], 0, MPI_COMM_WORLD, &status);
		for(int j = 0; j < numProcs; j++) {
			if(v[j] == -1) {
				v[j] = vRecv[j];
			}
		}
	}

	if (rank != leader) {
		MPI_Send(v, numProcs, MPI_INT, v[rank], 0, MPI_COMM_WORLD);
		MPI_Recv(v, numProcs, MPI_INT, v[rank], 0, MPI_COMM_WORLD, &status);
	}

	for (int i = 0; i < num_neigh; i++) {
		if (v[neigh[i]] != rank) continue;
		MPI_Send(v, numProcs, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
	}

	for (int i = 0; i < numProcs && rank == leader; i++) {
		printf("The node %d has the parent %d\n", i, v[i]);
	}

	return v;
}

int leader_chosing(int rank, int nProcesses) {
	int leader = -1;
	int q;
	leader = rank;
	
	/* Executam acest pas pana ajungem la convergenta */
	for (int k = 0; k < CONVERGENCE_COEF; k++) {
		for (int i = 0; i < num_neigh; i++) {
			MPI_Send(&rank, 1, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
			MPI_Recv(&q, 1, MPI_INT, neigh[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (q > leader) leader = q;
			
			MPI_Send(&leader, 1, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
			MPI_Recv(&q, 1, MPI_INT, neigh[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (q > leader) leader = q;
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	printf("%i/%i: leader is %i\n", rank, nProcesses, leader);

	return leader;
}

int get_number_of_nodes(int rank, int leader) {
	
	double val;
	if (leader == rank) {
		val = 1.0;
	} else {
		val = 0.0;
	}

	double recvd = 0;
	/* Executam acest pas pana ajungem la convergenta */
	for (int k = 0; k < CONVERGENCE_COEF; k++) {
		for (int i = 0; i < num_neigh; i++) {
			MPI_Send(&val, 1, MPI_DOUBLE, neigh[i], 2, MPI_COMM_WORLD);
			MPI_Recv(&recvd, 1, MPI_DOUBLE, neigh[i], 2, MPI_COMM_WORLD, NULL);
			val = (val + recvd) / 2;
		}
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	return (int)(1 / val);
}

int ** get_topology(int rank, int nProcesses, int * parents, int leader) {
	int ** topology = malloc(sizeof(int*) * nProcesses);
	int ** vTopology = malloc(sizeof(int*) * nProcesses);
	
	for (size_t i = 0; i < nProcesses; i++) {
		topology[i] = calloc(sizeof(int), nProcesses);
		vTopology[i] = calloc(sizeof(int), nProcesses);
	}

	for (size_t i = 0; i < num_neigh; i++) {
		topology[rank][neigh[i]] = 1;
	}

	for(int i = 0; i < num_neigh; i++) {
		if(parents[neigh[i]] == rank) {
			for(int j = 0; j < nProcesses; j++) {

				int *buffer = calloc(sizeof(int), nProcesses);
				MPI_Recv(buffer, nProcesses, MPI_INT, neigh[i], 0, MPI_COMM_WORLD, NULL);
				for(int k = 0; k < nProcesses; k++) {
					if(topology[j][k] == 0) {
						topology[j][k] = buffer[k];
					}
				}
				free(buffer);

			}
		}
	}

	if(rank != leader) {
		for (int i = 0; i < nProcesses; i++) {
			MPI_Send(topology[i], nProcesses, MPI_INT, parents[rank], 0, MPI_COMM_WORLD);
		}
	}

	if(rank != leader) {
		for (int i = 0; i < nProcesses; i++) {
			MPI_Recv(topology[i], nProcesses, MPI_INT, parents[rank], 0, MPI_COMM_WORLD, NULL);
		}
	}

	for (int i = 0; i < num_neigh; i++) {
		if (parents[neigh[i]] == rank) {
			for (int j = 0; j < nProcesses; j++) {
				MPI_Send(topology[j], nProcesses, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
			}
		}
	}

	return topology;
}

int main(int argc, char * argv[]) {
	int rank, nProcesses, num_procs, leader;
	int *parents, **topology;

	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Request request;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

	if (nProcesses != 12) {
		printf("please run with: mpirun --oversubscribe -np 12 %s\n", argv[0]);
		MPI_Finalize();	
		exit(0);
	}
	 
	read_neighbours(rank);
	leader = leader_chosing(rank, nProcesses);
	
	MPI_Barrier(MPI_COMM_WORLD);

	parents = get_dst(rank, nProcesses, leader);

	MPI_Barrier(MPI_COMM_WORLD);

	num_procs = get_number_of_nodes(rank, leader);
	
	printf("%d/%d There are %d processes\n", rank, nProcesses,num_procs);

	topology = get_topology(rank, nProcesses, parents, leader);

	for (size_t i = 0; i < nProcesses && rank == 0; i++)
	{
		for (size_t j = 0; j < nProcesses; j++)
		{
			printf("%2d ", topology[i][j]);	
		}
		printf("\n");
	}
	
	MPI_Finalize();
	return 0;
}