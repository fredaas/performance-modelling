/*
 * DESCRIPTION
 *
 * The ping-pong benchmark derives up to three levels of latency measurements,
 * namely intra-socket, inter-socket, and inter-node. The measurements are
 * stored as two adjacency matrices describing the setup latency and inverse
 * bandwidth between each core. Combined, these measurements yield the total
 * communication latency for a given message size.
 *
 * We assume the latency is the same in each direction of every two cores. Given
 * N cores, there are N * (N - 1) / 2 communication pairs.
 *
 * NOTE
 *
 * The message size must be set large enough to completely saturate the
 * communication channels in order to accurately describe the inverse bandwidth.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>

enum {
    T_ALPHA,
    T_BETA
};

typedef double real_t;

int rank, world_size;

double *restrict mat = NULL;

double *restrict p_mat = NULL;

real_t *restrict send_buff = NULL;
real_t *restrict recv_buff = NULL;

int *restrict tuples = NULL;
int num_tuples = 0;

const int MSG_SIZE_MAX  = 2 * (1 << 20); /* 64 * 2 * 1024^2 = 128 MiB */
const int MSG_SIZE_MIN  = 1;
const int NUM_BENCH_MIN = 10;
const int NUM_BENCH_MAX = 10000;

#define randf(a, b) ((a) + ((double)rand() / RAND_MAX) * ((b) - (a)))

/* Main matrix for gathering latencies */
#define T_MAT(k, i, j) \
    mat[(k) * world_size * world_size + world_size * (i) + (j)]

/* Local latency matrix */
#define T_PMAT(k, i) \
    p_mat[(k) * world_size + (i)]

/* Local communication pair matrix */
#define TUPLE(i, j) \
    tuples[2 * (i) + (j)]

void dump_matrix(void);
void free_resources(void);
void init(void);
void init_comm_tuples(void);
void mirror_matrix(void);
void ping_pong(int src, int dst, int msg_size, int num_bench, int k);
void ping_pong_bench(int msg_size, int num_bench, int k);
void print_comm_tuples(void);
double walltime(void);

int main(void)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (rank == 0)
    {
        int n = world_size;
        printf("Processes: %d\n", n);
        printf("Tuples: %d\n", (n * (n - 1) / 2));
    }

    init();

    MPI_Barrier(MPI_COMM_WORLD);

    double t_walltime = 0.0;

    if (rank == 0)
        t_walltime = walltime();

    ping_pong_bench(MSG_SIZE_MIN, NUM_BENCH_MAX, T_ALPHA);
    ping_pong_bench(MSG_SIZE_MAX, NUM_BENCH_MIN, T_BETA);

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
        printf("Latency: %.4lf\n", walltime() - t_walltime);

    MPI_Gather(
        &T_PMAT(T_ALPHA, 0), world_size, MPI_DOUBLE,
        &T_MAT(T_ALPHA, 0, 0), world_size, MPI_DOUBLE,
        0, MPI_COMM_WORLD
    );

    MPI_Gather(
        &T_PMAT(T_BETA, 0), world_size, MPI_DOUBLE,
        &T_MAT(T_BETA, 0, 0), world_size, MPI_DOUBLE,
        0, MPI_COMM_WORLD
    );

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
    {
        mirror_matrix();
        dump_matrix();
    }

    free_resources();

    MPI_Finalize();

    return 0;
}

double walltime(void)
{
    static struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1.0e-6;
}

void free_resources(void)
{
    free(send_buff);
    free(recv_buff);
    free(p_mat);
    free(mat);
    free(tuples);
}

void mirror_matrix(void)
{
    for (int k = 0; k < 2; k++)
        for (int i = 0; i < world_size; i++)
            for (int j = 0; j < world_size; j++)
                T_MAT(k, j, i) = T_MAT(k, i, j);
}

void dump_matrix(void)
{
    FILE *out[2] = {
        [T_ALPHA] = fopen("out_alpha.txt", "w+"),
        [T_BETA] = fopen("out_beta.txt", "w+")
    };
    for (int k = 0; k < 2; k++)
    {
        for (int i = 0; i < world_size; i++)
        {
            for (int j = 0; j < world_size; j++)
                fprintf(out[k], "%e ", T_MAT(k, i, j));
            fprintf(out[k], "\n");
        }
    }
}

void print_comm_tuples(void)
{
    if (rank == 0)
    {
        for (int i = 0; i < num_tuples; i++)
            printf("(%d %d) ", TUPLE(i, 0), TUPLE(i, 1));
        printf("\n");
    }
}

void init_comm_tuples(void)
{
    for (int i = world_size - 1; i >= 0; i--)
        num_tuples += i;

    tuples = (int *)calloc(num_tuples * 2, sizeof(int));

    int k = 0;
    for (int i = 0; i < world_size - 1; i++)
    {
        for (int j = i + 1; j < world_size; j++)
        {
            TUPLE(k, 0) = i;
            TUPLE(k, 1) = j;
            k++;
        }
    }
}

void init(void)
{
    init_comm_tuples();

    if (rank == 0)
        mat = (double *)malloc(2 * world_size * world_size * sizeof(double));

    p_mat = (double *)malloc(2 * world_size * sizeof(double));

    send_buff = (real_t *)calloc(MSG_SIZE_MAX, sizeof(real_t));
    recv_buff = (real_t *)calloc(MSG_SIZE_MAX, sizeof(real_t));

    for (int i = 0; i < MSG_SIZE_MAX; i++)
        send_buff[i] = randf(0, 9);
}

void ping_pong_bench(int msg_size, int num_bench, int k)
{
    for (int i = 0; i < num_tuples; i++)
    {
        int src = TUPLE(i, 0);
        int dst = TUPLE(i, 1);

        MPI_Barrier(MPI_COMM_WORLD);

        if ((rank == src) || (rank == dst))
            ping_pong(src, dst, msg_size, num_bench, k);
    }
}

void ping_pong(int src, int dst, int msg_size, int num_bench, int k)
{
    double start, end;

    if (rank == dst)
    {
        for (int _ = 0; _ < num_bench; _++)
        {
            MPI_Recv(recv_buff, msg_size, MPI_DOUBLE, src, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Ssend(recv_buff, msg_size, MPI_DOUBLE, src, 0,
                MPI_COMM_WORLD);
        }
    }
    else if (rank == src)
    {
        start = MPI_Wtime();
        for (int _ = 0; _ < num_bench; _++)
        {
            MPI_Ssend(send_buff, msg_size, MPI_DOUBLE, dst, 0,
                MPI_COMM_WORLD);
            MPI_Recv(recv_buff, msg_size, MPI_DOUBLE, dst, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        end = MPI_Wtime();
        T_PMAT(k, dst) = (end - start) / (2 * num_bench * msg_size * sizeof(real_t));
    }
}
