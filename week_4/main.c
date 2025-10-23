#include "mpi_helper.h"
#include <assert.h>
#include <math.h>

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
	MPI_Request _mpi_request = MPI_REQUEST_NULL;
	int _sum = value;

	for (int step = 0; step < _mpi_size-1; step++) {
		mpi_printf_once("Step: %d \n", step);
		int left_neighbor = get_left_neighbor_rank(_mpi_rank, _mpi_size);
		int right_neighbor = get_right_neighbor_rank(_mpi_rank, _mpi_size);

		int send_value = value;
		int recv_value;

		// Send to right neighbor and receive from left neighbor
		// MPI_Issend(&send_value, 1, MPI_INT, right_neighbor, 0, MPI_COMM_WORLD, &_mpi_request);
		// MPI_Recv(&recv_value, 1, MPI_INT, left_neighbor, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// MPI_Wait(&_mpi_request, MPI_STATUS_IGNORE);
		MPI_Sendrecv(&send_value, 1, MPI_INT, right_neighbor, 0,
					 &recv_value, 1, MPI_INT, left_neighbor, 0,
					 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		mpi_printf_once("Received value %d \n", recv_value);
		value = recv_value;
		_sum += value;
	}

	// sum(rank+1)^2 = (_mpi_size * (_mpi_size + 1) * (2 * _mpi_size + 1)) / 6
	// which is the sum of squares formula:
	// = P(P+1)(2P+1)/6, where P is number of ranks/procs
	mpi_printf("Total sum is %d \n", _sum);
	mpi_printf_once("Expected sum is %d \n", (_mpi_size * (_mpi_size + 1) * (2 * _mpi_size + 1)) / 6);
	assert(_sum == (_mpi_size * (_mpi_size + 1) * (2 * _mpi_size + 1)) / 6);
);
