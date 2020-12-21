#!/usr/bin/env bash

# DELLXPS
NUM_CORES=4 mpirun -n 4 ./comms1_saturate

# IDUN-E5
# NUM_CORES=20 NUM_TASKS=40 sbatch -N 2 --ntasks-per-node=20 -C pe630 -J "c1sat" -o "c1sat-%j.txt" -t 00:30:00 slurm.job
# NUM_CORES=28 NUM_TASKS=56 sbatch -N 2 --ntasks-per-node=28 -C pec6420 -J "c1sat" -o "c1sat-%j.txt" -t 00:30:00 slurm.job

# FRAM
# NUM_CORES=32 NUM_TASKS=64 sbatch -q devel -N 2 --ntasks-per-node=32 -J "c1sat" -o "c1sat-%j.txt" -t 00:30:00 slurm.job
