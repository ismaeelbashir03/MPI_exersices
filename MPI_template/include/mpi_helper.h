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