/*
 * The Hockney benchmark derives the coefficients of the Hockney communication
 * performance model,
 *
 *     t_comm = t_alpha + M * t_beta,
 *
 * where t_alpha [s] is the communication channel's setup time, t_beta [s /
 * byte] is the communication channel's inverse bandwidth, and M is the message
 * size in bytes.
 *
 * For each communication pair, msg_size elements are communicated between the
 * sending and receiving rank. Each element is transferred from the sender to
 * the receiver, and back again. We assume the latency is the same in both
 * directions. Thus the latency given by
 *
 *     (t_end - t_start) / (2 * num_bench * msg_size * sizeof(double)).
 *
 * t_alpha is approximated by setting num_bench high and msg_size low; t_beta is
 * approximated by setting num_bench high and msg_size low.
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

int *restrict pairs = NULL;
int num_pairs = 0;

MPI_Group group_world;

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
#define P_PMAT(i, j) \
    pairs[2 * (i) + (j)]

void create_comm_pair(int src, int dst, MPI_Comm *comm_pair);
void dump_matrix(void);
void free_resources(void);
void init(void);
void mirror_matrix(void);
void ping_pong(int src, int dst, int msg_size, int num_bench, int k);
void ping_pong_bench(int msg_size, int num_bench, int k);

int main(void)
{
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_group(MPI_COMM_WORLD, &group_world);

    init();

    ping_pong_bench(MSG_SIZE_MIN, NUM_BENCH_MAX, T_ALPHA);
    ping_pong_bench(MSG_SIZE_MAX, NUM_BENCH_MIN, T_BETA);

    MPI_Barrier(MPI_COMM_WORLD);

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

void free_resources(void)
{
    free(send_buff);
    free(recv_buff);
    free(p_mat);
    free(mat);
}

void create_comm_pair(int src, int dst, MPI_Comm *comm_pair)
{
    int pair[2] = { src, dst };
    MPI_Group group_pair;
    MPI_Group_incl(group_world, 2, pair, &group_pair);
    MPI_Comm_create_group(MPI_COMM_WORLD, group_pair, 0, comm_pair);
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

void init_comm_pairs(void)
{
    for (int i = world_size - 1; i >= 0; i--)
        num_pairs += i;

    pairs = (int *)calloc(num_pairs * 2, sizeof(int));

    int k = 0;
    for (int i = 0; i < world_size - 1; i++)
    {
        for (int j = i + 1; j < world_size; j++)
        {
            P_PMAT(k, 0) = i;
            P_PMAT(k, 1) = j;
            k++;
        }
    }

    if (rank == 0)
    {
        for (int i = 0; i < num_pairs; i++)
            printf("(%d %d) ", P_PMAT(i, 0), P_PMAT(i, 1));
        printf("\n");
    }
}

void init(void)
{
    init_comm_pairs();

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
    for (int i = 0; i < num_pairs; i++)
    {
        int src = P_PMAT(i, 0);
        int dst = P_PMAT(i, 1);

        if ((rank != src) && (rank != dst))
            continue;

        MPI_Comm comm_pair;
        create_comm_pair(src, dst, &comm_pair);
        MPI_Barrier(comm_pair);
        ping_pong(src, dst, msg_size, num_bench, k);
        MPI_Comm_free(&comm_pair);
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
