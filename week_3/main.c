#include "mpi_helper.h"

const int N = 12;

MPI_MAIN(

  	// give array of indices to rank 0 and make rest have -1 (no value)
	int[N] x;
	for (int i = 0; i < N; i++) {
		if (_mpi_rank == 0) x[i] = i; else x[i] = -1;
	}
	mpi_print_int_array(x, N);

	// broadcast to all other procs
	
	
);
