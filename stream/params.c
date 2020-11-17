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

    int L3_SIZE = atoi(getenv("L3_SIZE")) * 1024;

    double bytes = STREAM_ARRAY_SIZE * sizeof(real_t);

    printf("OPERAND SIZE: %d (should be at least 4 x times L3 cache)\n",
        STREAM_ARRAY_SIZE);

    printf("OPERAND MEMORY: %.2lf MiB (%.2lf x times L3 cache)\n",
        bytes / (float)(1024 * 1024), bytes / (double)L3_SIZE);

    printf("TOTAL MEMORY: %.2lf MiB (%.2lf x times L3 cache)\n",
        3 * bytes / (float)(1024 * 1024), 3 * bytes / (double)L3_SIZE);

    printf("L3 SIZE: %d KiB\n",
        L3_SIZE / 1024
    );
}

int main(void)
{
    print_params();

    return 0;
}
