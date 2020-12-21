#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>

typedef double real_t;

enum {T_ALPHA, T_BETA };

int rank, size;

MPI_Group world_group;

double *matx;
double *maty;

real_t *send_buff;
real_t *recv_buff;

int *pairs;
int num_pairs = 0;

int MSG_SIZE_MAX  = 12500000; /* 100 MiB */
int NUM_BENCH_MIN = 5;
int NUM_BENCH_MAX = 1000;

/* Root matrix */
#define A(k, i, j) matx[(k) * size * size + size * (i) + (j)]

/* Local vector */
#define B(k, i) maty[(k) * size + (i)]

/* Communication pairs */
#define TUPLE(i, j) pairs[2 * (i) + (j)]

void write_file(FILE *fp, int k)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            fprintf(fp, "%e ", A(k, i, j));
        }
        fprintf(fp, "\n");
    }
}

void write_matrix(void)
{
    /* Mirror matrix */
    for (int k = 0; k < 2; k++)
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                A(k, j, i) = A(k, i, j);

    char target[256];
    sprintf(target, "alpha_%d.txt", size);
    FILE *out_alpha = fopen(target, "w+");
    write_file(out_alpha, T_ALPHA);
    sprintf(target, "beta_%d.txt", size);
    FILE *out_beta = fopen(target, "w+");
    write_file(out_beta, T_BETA);
    fclose(out_alpha);
    fclose(out_beta);
}

void print_tuples(void)
{
    if (rank == 0)
    {
        for (int i = 0; i < num_pairs; i++)
            printf("(%d %d) ", TUPLE(i, 0), TUPLE(i, 1));
        printf("\n");
    }
}

void create_tuples(void)
{
    num_pairs = (size * (size - 1) / 2);
    pairs = (int *)calloc(num_pairs * 2, sizeof(int));
    int k = 0;
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = i + 1; j < size; j++)
        {
            TUPLE(k, 0) = i;
            TUPLE(k, 1) = j;
            k++;
        }
    }
}

void initialize_buffers(void)
{
    create_tuples();

    if (rank == 0)
        matx = (double *)calloc(2 * size * size, sizeof(double));
    maty = (double *)calloc(2 * size, sizeof(double));
    send_buff = (real_t *)calloc(MSG_SIZE_MAX, sizeof(real_t));
    recv_buff = (real_t *)calloc(MSG_SIZE_MAX, sizeof(real_t));
    double j = 0.0;
    for (int i = 0; i < MSG_SIZE_MAX; i++)
        send_buff[i] = ++j;
}

void comms1(int src, int dst, int message_size, int num_bench, int k)
{
    double start, end;

    if (rank == dst)
    {
        for (int x = 0; x < num_bench; x++)
        {
            MPI_Recv(recv_buff, message_size, MPI_DOUBLE, src, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(recv_buff, message_size, MPI_DOUBLE, src, 0,
                MPI_COMM_WORLD);
        }
    }
    else if (rank == src)
    {
        start = MPI_Wtime();
        for (int x = 0; x < num_bench; x++)
        {
            MPI_Send(send_buff, message_size, MPI_DOUBLE, dst, 0,
                MPI_COMM_WORLD);
            MPI_Recv(recv_buff, message_size, MPI_DOUBLE, dst, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        end = MPI_Wtime();
        if (k == T_ALPHA)
            B(k, dst) = (end - start) / num_bench;
        else
            B(k, dst) = (end - start) / (2 * num_bench * message_size * sizeof(real_t));
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

void run_comms1(int message_size, int num_bench, int k)
{
    for (int i = 0; i < num_pairs; i++)
    {
        int src = TUPLE(i, 0);
        int dst = TUPLE(i, 1);

        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == src || rank == dst)
        {
            MPI_Comm COMM_PAIR;
            create_communicator(src, dst, &COMM_PAIR);
            MPI_Barrier(COMM_PAIR);
            comms1(src, dst, message_size, num_bench, k);
            MPI_Comm_free(&COMM_PAIR);
        }
    }
}

int main(void)
{
    srand(1234);

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    if (rank == 0)
    {
        printf("Ranks: %d\n", size);
        printf("Pairs: %d\n", (size * (size - 1) / 2));
    }

    initialize_buffers();

    print_tuples();

    MPI_Barrier(MPI_COMM_WORLD);

    run_comms1(0, NUM_BENCH_MAX, T_ALPHA);
    run_comms1(MSG_SIZE_MAX, NUM_BENCH_MIN, T_BETA);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Gather(
        &B(T_ALPHA, 0), size, MPI_DOUBLE,
        &A(T_ALPHA, 0, 0), size, MPI_DOUBLE,
        0, MPI_COMM_WORLD
    );

    MPI_Gather(
        &B(T_BETA, 0), size, MPI_DOUBLE,
        &A(T_BETA, 0, 0), size, MPI_DOUBLE,
        0, MPI_COMM_WORLD
    );

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
        write_matrix();

    MPI_Finalize();

    return 0;
}
