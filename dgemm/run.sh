#!/usr/bin/env bash

# Note: Do NOT set OMP_SCHEDULE, OMP_PLACES, or OMP_PROC_BIND as it degrades
# openblas performance.

# DELLXPS
echo "# cores min, max, avg [gigaFLOPS]"
for i in `seq 1 4`; do NUM_BENCH=2 OMP_NUM_THREADS=$i ./dgemm; done

# FRAM
# sbatch -q devel -J "dgemm" -o "dgemm.txt" -t 00:20:00 slurm.job

# IDUN
# sbatch -C pe630 -J "dgemm-e5" -o "dgemm-e5-%j.txt" -t 00:20:00 slurm.job
# sbatch -C pec6420 -J "dgemm-gold" -o "dgemm-gold-%j.txt" -t 00:20:00 slurm.job
