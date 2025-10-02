#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

/*
MPI print to only print with processor 0 (no verbose logging)
*/
#define mpi_printf(...) { \
  int rank; \
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
  if (rank == 0) { \
    printf(__VA_ARGS__); \
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