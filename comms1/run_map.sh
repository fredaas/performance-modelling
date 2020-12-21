#!/usr/bin/env bash

# DELLXPS
mpirun -n 4 ./comms1_map

# IDUN-E5
# NUM_TASKS=120 sbatch -N 6 --ntasks-per-node=20 -C pe630 -J "comms1" -o "comms1-%j.txt" -t 03:00:00 slurm.job
# NUM_TASKS=112 sbatch -N 4 --ntasks-per-node=28 -C pec6420 -J "comms1" -o "comms1-%j.txt" -t 03:00:00 slurm.job

# FRAM
# NUM_TASKS=128 sbatch -N 4 --ntasks-per-node=32 -J "comms1" -o "comms1-%j.txt" -t 03:00:00 slurm.job
