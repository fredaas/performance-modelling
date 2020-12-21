#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <omp.h>

#ifdef OPENBLAS
#include <cblas.h>
#endif

/* Operand unit type */
typedef double real_t;

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define rand_float(a, b) ((a) + ((real_t)rand() / RAND_MAX) * ((b) - (a)))

real_t *mat_a;
real_t *mat_b;
real_t *mat_c;

real_t alpha = 1.0;
real_t beta = 2.0;

/* LDA is the leading dimension */
#define A(i, j, lda) mat_a[(i) * (lda) + (j)]
#define B(i, j, lda) mat_b[(i) * (lda) + (j)]
#define C(i, j, lda) mat_c[(i) * (lda) + (j)]

#define M 10000
#define N 1000
#define Q 1000

int NUM_BENCH = 10;

double t_start, t_time, t_min, t_max, t_avg;

int num_threads;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

double walltime(void)
{
    static struct timeval t;
    gettimeofday(&t, NULL);
    return ((double)t.tv_sec + (double)t.tv_usec * 1.0e-06);
}

void fill_mat(real_t *m, int rows, int cols)
{
    for (int i = 0; i < rows * cols; i++)
    {
        int x = i % cols;
        int y = i / cols;
        m[y * cols + x] = rand_float(0, 9);
    }
}

void reset_timers(void)
{
    t_start = 0;
    t_time = 0;
    t_min = DBL_MAX;
    t_max = 0;
    t_avg = 0;
}

void dgemm_cblas(void)
{
    static int iteration = 0;

    t_start = walltime();

#ifdef OPENBLAS
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
        M, Q, N, alpha, mat_a, N, mat_b, Q, beta, mat_c, Q);
#endif

    if (iteration++ == 0)
        return;

    t_time = walltime() - t_start;
    t_min = MIN(t_min, t_time);
    t_max = MAX(t_max, t_time);
    t_avg += t_time;
}

void dgemm_omp(void)
{
    t_start = walltime();
    #pragma omp parallel for
    for (int ra = 0; ra < M; ra++)
    {
        for (int cb = 0; cb < Q; cb++)
        {
            real_t sum = 0;
            for (int rb = 0; rb < N; rb++)
            {
                sum += A(ra, rb, N) * B(rb, cb, N);
            }
            C(ra, cb, Q) = sum;
        }
    }
    t_time = walltime() - t_start;
    t_min = MIN(t_min, t_time);
    t_max = MAX(t_max, t_time);
    t_avg += t_time;
}

void dgemm_vanilla(void)
{
    t_start = walltime();
    for (int ra = 0; ra < M; ra++)
    {
        for (int cb = 0; cb < Q; cb++)
        {
            real_t sum = 0;
            for (int rb = 0; rb < N; rb++)
            {
                sum += A(ra, rb, N) * B(rb, cb, N);
            }
            C(ra, cb, Q) = sum;
        }
    }
    t_time = walltime() - t_start;
    t_min = MIN(t_min, t_time);
    t_max = MAX(t_max, t_time);
    t_avg += t_time;
}

int compare(real_t *a, real_t *b, int rows, int cols)
{
    for (int i = 0; i < rows * cols; i++)
    {
        int x = i % cols;
        int y = i / cols;
        int index = y * cols + x;
        if (a[index] != b[index])
            return 0;
    }
    return 1;
}

void print_results(void)
{
    t_avg /= (NUM_BENCH - 1);
    printf("%d %.1lf %.1lf %.1lf\n",
        num_threads,
        2 * M * N * (Q * 1.0E-9) / t_max,
        2 * M * N * (Q * 1.0E-9) / t_min,
        2 * M * N * (Q * 1.0E-9) / t_avg
    );
}

void init_globals(void)
{
    if (getenv("NUM_BENCH"))
        NUM_BENCH = atoi(getenv("NUM_BENCH"));

    if (NUM_BENCH < 2)
    {
        printf("[ERROR] NUM_BENCH to small\n");
        exit(1);
    }
}

int main(int argc, char **argv)
{
    srand(1234);

    init_globals();

    mat_a = (real_t *)malloc(M * N * sizeof(real_t));
    mat_b = (real_t *)malloc(N * Q * sizeof(real_t));
    mat_c = (real_t *)malloc(M * Q * sizeof(real_t));

    #pragma omp parallel
    {
        #pragma omp master
        num_threads = omp_get_num_threads();
    }

    fill_mat(mat_a, M, N);
    fill_mat(mat_b, N, Q);
    fill_mat(mat_c, M, Q);

    reset_timers();
    for (int i = 0; i < NUM_BENCH; i++)
        dgemm_cblas();
    print_results();

	return 0;
}
