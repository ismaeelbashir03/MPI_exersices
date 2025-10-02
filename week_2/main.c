#include "mpi_helper.h"
#include <math.h>

const int N = 1000000; // 1 millon intervals to sample from

void estimate_pi(int _mpi_rank, int _mpi_size, double *result) {
	int comp_per_rank = N / _mpi_size;
	int remainder = N % _mpi_size;
    
    // determine start and end indices for each rank
	// if n is not divisible by size, the first 'remainder' ranks get one extra computation 
	// (spread the extra between them)
    int start, end;
    if (_mpi_rank < remainder) {
        start = _mpi_rank * (comp_per_rank + 1);
        end = start + comp_per_rank + 1;
    } else {
        start = _mpi_rank * comp_per_rank + remainder;
        end = start + comp_per_rank;
    }

	double local_sum = 0.0;
	// each rank starts from its index (given by _mpi_rank) times the number of computations per rank
	// and goes up to the next rank's starting index ((_mpi_rank + 1) * comp_per_rank)
	for (int i = start; i < end; i++) {
		double x = (i-0.5)/N;
		local_sum += 1. / (1 + pow(x, 2));
	}
	
	// rank 0 collects all results and prints the final estimation (can use MPI_Reduce instead but spec asks for MPI_Send and MPI_Recv)
	if (_mpi_rank == 0) {
		double global_sum = local_sum;
		for (int source = 1; source < _mpi_size; source++) {
			double recv_sum;
			MPI_Recv(&recv_sum, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			global_sum += recv_sum;
		}
		*result = global_sum * 4.0 / N;
	} else {
		MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
}

void estimate_pi_recv_wildcard_tags(int mpi_rank, int mpi_size, double *result) {
	int comp_per_rank = N / mpi_size;
	int remainder = N % mpi_size;
	
	// determine start and end indices for each rank
	// if n is not divisible by size, the first 'remainder' ranks get one extra computation 
	// (spread the extra between them)
	int start, end;
	if (mpi_rank < remainder) {
		start = mpi_rank * (comp_per_rank + 1);
		end = start + comp_per_rank + 1;
	} else {
		start = mpi_rank * comp_per_rank + remainder;
		end = start + comp_per_rank;
	}

	double local_sum = 0.0;
	// each rank starts from its index (given by _mpi_rank) times the number of computations per rank
	// and goes up to the next rank's starting index ((_mpi_rank + 1) * comp_per_rank)
	for (int i = start; i < end; i++) {
		double x = (i-0.5)/N;
		local_sum += 1. / (1 + pow(x, 2));
	}
	
	// rank 0 collects all results and prints the final estimation (can use MPI_Reduce instead but spec asks for MPI_Send and MPI_Recv)
	if (mpi_rank == 0) {

        // Note: we can make this more efficient by making each rank send its result with its rank as the tag, \
		// then we can process them in order
		double global_sum = local_sum;
		for (int source = 1; source < mpi_size; source++) {
			double recv_sum;
			// passing source as tag so we wait for that specific message before proceeding
			MPI_Recv(&recv_sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, source, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			global_sum += recv_sum;
		}

		*result = global_sum * 4.0 / N;
    } else {
        MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, mpi_rank, MPI_COMM_WORLD);
    }
}

void estimate_pi_recv_wildcard(int mpi_rank, int mpi_size, double *result) {
	int comp_per_rank = N / mpi_size;
	int remainder = N % mpi_size;
	
	// determine start and end indices for each rank
	// if n is not divisible by size, the first 'remainder' ranks get one extra computation 
	// (spread the extra between them)
	int start, end;
	if (mpi_rank < remainder) {
		start = mpi_rank * (comp_per_rank + 1);
		end = start + comp_per_rank + 1;
	} else {
		start = mpi_rank * comp_per_rank + remainder;
		end = start + comp_per_rank;
	}

	double local_sum = 0.0;
	// each rank starts from its index (given by _mpi_rank) times the number of computations per rank
	// and goes up to the next rank's starting index ((_mpi_rank + 1) * comp_per_rank)
	for (int i = start; i < end; i++) {
		double x = (i-0.5)/N;
		local_sum += 1. / (1 + pow(x, 2));
	}
	
	// rank 0 collects all results and prints the final estimation (can use MPI_Reduce instead but spec asks for MPI_Send and MPI_Recv)
	if (mpi_rank == 0) {
		// Note: we need to allocate an array for all local sums to receive them in order
		// if we dont do this, then we can get non-deterministic results since float summation is not associative due to rounding errors
        double *all_sums = malloc(mpi_size * sizeof(double));
        all_sums[0] = local_sum;

        for (int i = 1; i < mpi_size; i++) {
            double recv_sum;
            MPI_Status status;
            MPI_Recv(&recv_sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            int sender = status.MPI_SOURCE;
            all_sums[sender] = recv_sum;
        }

        double global_sum = 0.0;
        for (int i = 0; i < mpi_size; i++) {
            global_sum += all_sums[i];
        }
        free(all_sums);

        *result = global_sum * 4.0 / N;
    } else {
        MPI_Send(&local_sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
}

void do_n_times(int _mpi_rank, int _mpi_size, int n, double *result, void (*func)(int, int, double*)) {
	// repeat the estimation n times so we can time it better (should be above 1 second for reliable timing)
	for (int i = 0; i < n; i++) {
		func(_mpi_rank, _mpi_size, result);
	}
}

MPI_MAIN(
  	double pi_estimate;
	mpi_printf_once("================================\n");
	mpi_printf_once("Estimating pi using with specific sender and recieve\n");
	mpi_printf_once("================================\n");
	mpi_time(5,
		do_n_times(_mpi_rank, _mpi_size, 1000, &pi_estimate, estimate_pi);
	);
	mpi_printf_once("Estimated value of pi: %f\n", pi_estimate);

	mpi_printf_once("================================\n");
	mpi_printf_once("Estimating pi using wildcard recieve with array\n");
	mpi_printf_once("================================\n");
	mpi_time(5,
		do_n_times(_mpi_rank, _mpi_size, 1000, &pi_estimate, estimate_pi_recv_wildcard);
	);
	mpi_printf_once("Estimated value of pi: %f\n", pi_estimate);

	mpi_printf_once("================================\n");
	mpi_printf_once("Estimating pi using wildcard recieve with tags\n");
	mpi_printf_once("================================\n");
	mpi_time(5,
		do_n_times(_mpi_rank, _mpi_size, 1000, &pi_estimate, estimate_pi_recv_wildcard_tags);
	);
	mpi_printf_once("Estimated value of pi: %f\n", pi_estimate);

);
