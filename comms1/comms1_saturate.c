#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>

typedef double real_t;

int rank, size;

MPI_Group world_group;

real_t *send_buff = NULL;
real_t *recv_buff = NULL;

int MSG_SIZE_MAX  = 100000000; /* 100 MiB */

int num_pairs = 0;

int pairs[3 * 2];

#define TUPLE(i, j) pairs[(i) * 2 + (j)]

/* Number of cores per node */
int num_cores = 0;

int num_bench = 10;

void print_tuples(void)
{
    if (rank == 0)
    {
        for (int i = 0; i < 3; i++)
            printf("(%d %d) ", TUPLE(i, 0), TUPLE(i, 1));
        printf("\n");
    }
}

void initialize_buffers(void)
{
    recv_buff = (real_t *)calloc(MSG_SIZE_MAX, sizeof(real_t));
    if (rank == 0)
    {
        send_buff = (real_t *)calloc(MSG_SIZE_MAX, sizeof(real_t));
        double j = 0.0;
        for (int i = 0; i < MSG_SIZE_MAX; i++)
            send_buff[i] = ++j;
    }
}

double comms1(int src, int dst, int message_size)
{
    double start, end;

    if (rank == dst)
    {
        for (int x = 0; x < num_bench; x++)
        {
            MPI_Recv(recv_buff, message_size, MPI_DOUBLE, src, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Ssend(recv_buff, message_size, MPI_DOUBLE, src, 0,
                MPI_COMM_WORLD);
        }
    }
    else if (rank == src)
    {
        start = MPI_Wtime();
        for (int x = 0; x < num_bench; x++)
        {
            MPI_Ssend(send_buff, message_size, MPI_DOUBLE, dst, 0,
                MPI_COMM_WORLD);
            MPI_Recv(recv_buff, message_size, MPI_DOUBLE, dst, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        end = MPI_Wtime();

        return 1.0E-9 * 2 * (num_bench * message_size) * sizeof(real_t) / (end - start);
    }
}

void create_communicator(int src, int dst, MPI_Comm *COMM_PAIR)
{
    int pair[2] = { src, dst };
    MPI_Group group;
    /* Create group */
    MPI_Group_incl(world_group, 2, pair, &group);
    /* Create communicator */
    MPI_Comm_create_group(MPI_COMM_WORLD, group, 0, COMM_PAIR);
}

int main(int argc, char **argv)
{
    srand(1234);

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    if (getenv("NUM_CORES"))
    {
        num_cores = atoi(getenv("NUM_CORES"));
    }
    else
    {
        printf("[ERROR] NUM_CORES not set\n");
        exit(1);
    }

    initialize_buffers();

    TUPLE(0, 0) = 0; TUPLE(0, 1) = 2;
    TUPLE(1, 0) = 0; TUPLE(1, 1) = 1;
    TUPLE(2, 0) = 0; TUPLE(2, 1) = num_cores;

    MPI_Barrier(MPI_COMM_WORLD);

    print_tuples();

    int latency[3] = { 0.0, 0.0, 0.0 };

    int max_message_size = MSG_SIZE_MAX;

    char *label[] = { "intra-socket", "inter-socket", "inter-node" };

    for (int i = 0; i < 3; i++)
    {
        int src = TUPLE(i, 0);
        int dst = TUPLE(i, 1);

        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == src || rank == dst)
        {
            MPI_Comm COMM_PAIR;
            create_communicator(src, dst, &COMM_PAIR);
            MPI_Barrier(COMM_PAIR);

            if (rank == 0)
                printf("%s\n", label[i]);

            for (int n = 1000000; n <= max_message_size; n += 1000000)
            {
                double bw = comms1(src, dst, n);
                if (rank == src)
                    printf("    %d %e\n", n, bw);
            }

            MPI_Comm_free(&COMM_PAIR);
        }
    }

    MPI_Finalize();

    return 0;
}
