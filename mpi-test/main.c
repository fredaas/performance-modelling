#include <mpi.h>
#include <stdio.h>

int size, rank;

int main(void)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (int r = 0; r < size; r++)
    {
        if (r == rank)
            printf("CORE %d / %d\n", rank + 1, size);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
