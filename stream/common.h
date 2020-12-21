/* Operand unit type */
typedef double real_t;

/* Number of benchmark iterations */
#ifndef NUM_BENCH
#define NUM_BENCH 10
#endif

enum { COPY, SCALE, ADD, TRIAD };
enum { MIN, MAX, AVG };
