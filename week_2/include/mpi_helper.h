#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

/*
Function to get string prefix for the current MPI rank (for printing)
*/
#define mpi_rank_prefix() { \
  int rank; \
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
  static char prefix[20]; \
  snprintf(prefix, sizeof(prefix), "[Rank %d] ", rank); \
  prefix; \
}

/*
MPI print to only print with processor 0 (no verbose logging)
*/
#define mpi_printf_once(...) { \
  int rank; \
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
  if (rank == 0) { \
    printf(__VA_ARGS__); \
  } \
}

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