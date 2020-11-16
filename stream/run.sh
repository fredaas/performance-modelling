#!/usr/bin/env bash

export OMP_SCHEDULE="guided"

unset KMP_AFFINITY

echo "COMPACT"
for i in `seq 1 8`; do OMP_PLACES="cores" OMP_PROC_BIND="close" OMP_NUM_THREADS=$i ./stream; done

echo "SCATTER"
for i in `seq 1 8`; do OMP_PLACES="cores" OMP_PROC_BIND="spread" OMP_NUM_THREADS=$i ./stream; done
