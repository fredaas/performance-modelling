#!/usr/bin/env bash

export CFLAGS="-std=c99 -Wall -O2 "

case $1 in
"dellxps")
    CC=gcc
    CFLAGS+="-fopenmp "
    ;;
"idun")
    module load GCC/9.3.0 OpenBLAS/0.3.9
    CC=gcc
    CFLAGS+="-fopenmp "
    ;;
"fram")
    module load OpenBLAS/0.3.9-GCC-9.3.0
    CC=gcc
    CFLAGS+="-fopenmp "
    ;;
*)
    echo "Invalid label"
    ;;
esac
