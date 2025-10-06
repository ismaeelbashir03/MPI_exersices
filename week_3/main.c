#include "mpi_helper.h"
#include <stdio.h>

const int N = 20;

MPI_MAIN(

  	// // give array of indices to rank 0 and make rest have -1 (no value)
	// int x[N];
	// for (int i = 0; i < N; i++) {
	// 	x[i] = (_mpi_rank == 0) ? i : -1;
	// }

	// mpi_printf_once("Before broadcast:\n");
	// mpi_print_int_array(x, N);
	// MPI_Barrier(MPI_COMM_WORLD);

	// // broadcast to all other procs
	// my_mpi_broadcast(x, N, MPI_INT, 0, NULL, MPI_COMM_WORLD);

	// MPI_Barrier(MPI_COMM_WORLD);
	// mpi_printf_once("After broadcast:\n");
	// mpi_print_int_array(x, N);

	// same array but for scatter
	MPI_Barrier(MPI_COMM_WORLD);
	mpi_printf_once("Scatter example:\n");
	int y[N];
	for (int i = 0; i < N; i++) {
		y[i] = (_mpi_rank == 0) ? i : -1;
	}

	// scatter will send sections of the array to each proc
	// so we need to get the size of each section
	int section_size = N / _mpi_size;
	int *section = (int *)malloc(section_size * sizeof(int));
	my_mpi_scatter(y, section_size, MPI_INT, section, section_size, MPI_COMM_WORLD);

	mpi_print_int_array(section, section_size);
	free(section);

);
