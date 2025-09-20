#include "mpi_helper.h"

int main(void)
{
  MPI_Init(NULL, NULL);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  mpi_printf("Hello this is %d, with %d MPI tasks\n", rank, size);

  char name[MPI_MAX_PROCESSOR_NAME];
  int length = 0;
  MPI_Get_processor_name(name, &length);
  
  if (length >= 0 && length < MPI_MAX_PROCESSOR_NAME) {
    name[length] = '\0';
  } else {
    name[MPI_MAX_PROCESSOR_NAME - 1] = '\0';
  }
  printf("Rank %d is on %s\n", rank, name);

  MPI_Finalize();
  return 0;
}
