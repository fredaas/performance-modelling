#!/usr/bin/env bash

CFLAGS="-std=c99 -Wall -O2 "

case $1 in
"local")
    export CC=mpicc
    export CFLAGS="-fopenmp "
    ;;
"slurm")
    module load intel/2018b
    export CC=icc
    export CFLAGS+="-qopenmp -lmpi "
    ;;
*)
    echo "[ERROR] Invalid label"
    ;;
esac
