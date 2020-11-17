/* Operand unit type */
typedef double real_t;

/* Number of benchmark iterations */
#ifndef NUM_BENCH
#define NUM_BENCH 10
#endif

#ifndef STREAM_ARRAY_SIZE
#define STREAM_ARRAY_SIZE 10000000
#endif

enum { COPY, SCALE, ADD, TRIAD };
enum { MIN, MAX, AVG };
