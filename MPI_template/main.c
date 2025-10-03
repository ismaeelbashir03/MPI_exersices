#include "mpi_helper.h"

MPI_MAIN(
  mpi_printf_once("Hello this is %d, with %d MPI tasks\n", _mpi_rank, _mpi_size);

  mpi_printf("I have rank %d\n", _mpi_rank);
);
