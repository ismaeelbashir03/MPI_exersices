#include "mpi_helper.h"
#include <assert.h>
#include <math.h>

const char SAVE_FILE_NAME[] = "output.txt";

int get_right_neighbor_rank(int rank, int size) {
	return (rank + 1) % size;
}

int get_left_neighbor_rank(int rank, int size) {
	return (rank - 1 + size) % size;
}

int get_init_value(int rank) {
	return pow(2, rank+1); // (rank+1)^2
}

MPI_MAIN(
	// each rank starts with its own rank as the value
	// int value = rank;
	int value = get_init_value(_mpi_rank);
	int _sum = value;

	// global sum of all ranks' values using mpi collective communication
	// takes log(p) time where p is number of ranks/procs, by using binomial tree algorithm (i think ? lol)
	MPI_Allreduce(&value, &_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	// sum(rank+1)^2 = (_mpi_size * (_mpi_size + 1) * (2 * _mpi_size + 1)) / 6
	// which is the sum of squares formula:
	// = P(P+1)(2P+1)/6, where P is number of ranks/procs
	mpi_printf("Total sum is %d \n", _sum);
	mpi_printf_once("Expected sum is %d \n", (_mpi_size * (_mpi_size + 1) * (2 * _mpi_size + 1)) / 6);
	assert(_sum == (_mpi_size * (_mpi_size + 1) * (2 * _mpi_size + 1)) / 6);

	// write to file in order
	for (int i = 0; i < _mpi_size; i++) {
		if (i == _mpi_rank) {
			FILE *fp = fopen(SAVE_FILE_NAME, "a");
			if (fp == NULL) {
				mpi_printf_once("Error opening file %s\n", SAVE_FILE_NAME);
				MPI_Abort(MPI_COMM_WORLD, 1);
			}
			fprintf(fp, "Rank %d: init value = %d, sum = %d\n", _mpi_rank, value, _sum);
			fclose(fp);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
);
