#include <mpi.h>
#include <stdio.h>

int size, rank;

int main(void)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("%d / %d\n", rank, size);

    MPI_Finalize();

    return 0;
}
