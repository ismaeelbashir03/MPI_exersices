#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>

/*
 * A custom implementation of broadcast
 *
 * buffer: pointer to data to be broadcasted
 * count: number of elements in the buffer
 * datatype: MPI datatype of the elements in the buffer
 * src: rank of the source processor
 * dsts: array of destination ranks (can be NULL to broadcast to all)
 * comm: MPI communicator
 */
int my_mpi_broadcast(void *buffer, int count, MPI_Datatype datatype, int src, int *dsts, MPI_Comm comm) {
	int rank, size;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &size);

	// if user did not provide destinations, we can just send to all
	if (dsts == NULL) {
		if (rank == src) {
			for (int i = 0; i < size; i++) {
				if (i != src) {
					MPI_Send(buffer, count, datatype, i, 0, comm);
				}
			}
		} else {
			MPI_Recv(buffer, count, datatype, src, 0, comm, MPI_STATUS_IGNORE);
		}
	} else {
		int is_dst = 0;
		for (int i = 0; dsts[i] != -1; i++) {
			if (rank == dsts[i]) {
				is_dst = 1;
				break;
			}
		}

		if (rank == src) {
			for (int i = 0; dsts[i] != -1; i++) {
				MPI_Send(buffer, count, datatype, dsts[i], 0, comm);
			}
		} else if (is_dst) {
			MPI_Recv(buffer, count, datatype, src, 0, comm, MPI_STATUS_IGNORE);
		}
	}

	return 0;
}

/*
 * A custom implementation of scatter
 * 
 * sendbuf: pointer to data to be sent (only significant at root)
 * sendcount: number of elements sent to each process
 * sendtype: MPI datatype of the elements in the send buffer
 * recvbuf: pointer to buffer to receive data (significant at all processes)
 * recvcount: number of elements in the receive buffer
 * comm: MPI communicator
 */
int my_mpi_scatter(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == 0) {
      for (int i = 0; i < size; i++) {
        if (i == 0) {
          // rank 0 just copies from sendbuf to recvbuf
          memcpy(recvbuf, sendbuf, sendcount * sizeof(sendtype));
          continue;
        }
        // we should send the section that corrosponds to the rank (for example rank 2 gets section 2 of buffer)
        int typesize;
        MPI_Type_size(sendtype, &typesize);
        char *shifted_buffer = (char *)sendbuf + (i * sendcount) * typesize;
        MPI_Send(shifted_buffer, sendcount, sendtype, i, 0, comm);
      }
    } else {
    	MPI_Recv(recvbuf, recvcount, sendtype, 0, 0, comm, MPI_STATUS_IGNORE);
	}
    return 0;
  }

/*
 * Function to get string prefix for the current MPI rank (for printing)
 */
#define mpi_rank_prefix() { \
  int rank; \
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
  static char prefix[20]; \
  snprintf(prefix, sizeof(prefix), "[Rank %d] ", rank); \
  prefix; \
}


/*
 * MPI print array with all processors
 */
#define mpi_print_int_array(x, size) { \
	mpi_printf("Array: "); \
	for (int i = 0; i < size; i++) { \
		printf("%d ", x[i]); \
	} \
  printf("\n"); \
} \

/*
 * MPI print to only print with processor 0 (no verbose logging)
 */
#define mpi_printf_once(...) { \
  int rank; \
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
  if (rank == 0) { \
    printf(__VA_ARGS__); \
  } \
}

/*
 * MPI print with all processors
 */
#define mpi_printf(...) { \
  int rank; \
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
  printf("[Rank %d] ", rank); \
  printf(__VA_ARGS__); \
}

/*
 * Macro to time a section of code.
 * Usage:
 *   mpi_time( number of samples,
 *       // code to time here
 *   );
 */
#define mpi_time(samples, ...) { \
  double start_time, end_time; \
  double total_time = 0.0; \
  for (int i = 0; i < samples; i++) { \
    MPI_Barrier(MPI_COMM_WORLD); \
    start_time = MPI_Wtime(); \
    __VA_ARGS__ \
    MPI_Barrier(MPI_COMM_WORLD); \
    end_time = MPI_Wtime(); \
    total_time += (end_time - start_time); \
  } \
  if (_mpi_rank == 0) { \
    if (samples > 1) { \
      mpi_printf("Average time over %d samples: %f seconds\n", samples, total_time / samples); \
    } else { \
      mpi_printf("Time: %f seconds\n", total_time); \
    } \
  } \
}

/* 
 * Macro to wrap MPI boilerplate around the user’s main code.
 * Usage:
 *   MPI_MAIN(int argc, char **argv) {
 *       // user code here
 *   }
 */
#define MPI_MAIN(...) \
    int main(int argc, char **argv) { \
        MPI_Init(&argc, &argv); \
        int _mpi_rank, _mpi_size; \
        MPI_Comm_rank(MPI_COMM_WORLD, &_mpi_rank); \
        MPI_Comm_size(MPI_COMM_WORLD, &_mpi_size); \
        (void)_mpi_rank; (void)_mpi_size; /* silence unused warnings */ \
        __VA_ARGS__ \
        MPI_Finalize(); \
        return 0; \
    }