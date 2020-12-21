#include <stdio.h>
#include <stdlib.h>

#include "common.h"

void print_params(void)
{
    if (!getenv("L3_SIZE"))
    {
        printf("[ERROR] L3_SIZE not set\n");
        exit(1);
    }

    if (!getenv("STREAM_ARRAY_SIZE"))
    {
        printf("[ERROR] STREAM_ARRAY_SIZE not set\n");
        exit(1);
    }

    if (!getenv("NUM_CPUS"))
    {
        printf("[ERROR] NUM_CPUS not set\n");
        exit(1);
    }

    int STREAM_ARRAY_SIZE = atoi(getenv("STREAM_ARRAY_SIZE"));
    int NUM_CPUS = atoi(getenv("NUM_CPUS"));
    int L3_SIZE = NUM_CPUS * atoi(getenv("L3_SIZE")) * 1024;

    double bytes = STREAM_ARRAY_SIZE * sizeof(real_t);

    printf("OPERAND SIZE: %d (should be at least 4 x times L3 cache)\n",
        STREAM_ARRAY_SIZE);

    printf("OPERAND MEMORY: %.2lf MiB (%.2lf x times L3 cache)\n",
        bytes / (double)(1024 * 1024), bytes / (double)(NUM_CPUS * L3_SIZE));

    printf("TOTAL MEMORY: %.2lf MiB (%.2lf x times L3 cache)\n",
        3 * bytes / (double)(1024 * 1024), 3 * bytes / (double)(NUM_CPUS * L3_SIZE));

    printf("L3 SIZE: %d KiB\n",
        L3_SIZE / 1024
    );
}

int main(void)
{
    print_params();

    return 0;
}
