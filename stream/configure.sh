#!/usr/bin/env bash

system=$1

CFLAGS="-std=c99 -Wall -O2 -I./ -Wno-unused-but-set-variable "

case $1 in
# Intel Core i7-7700HQ, 8 cores, 6144 KB cache
"dellxps")
    export CC=gcc
    export CFLAGS+="-fopenmp "
    ;;
# Intel Xeon E5-2630 v2, 10 cores, 25000 KB cache
"idun-e5")
    module load intel
    export CC=icc
    export CFLAGS+="-qopenmp -DSTREAM_ARRAY_SIZE=80000000 "
    ;;
# Intel Xeon Gold 6132, 14 cores, 19000 KB cache
"idun-gold")
    module load intel
    export CC=icc
    export CFLAGS+="-qopenmp -DSTREAM_ARRAY_SIZE=80000000 "
    ;;
# Intel Xeon E5-2683 v4, 16 cores, 40000 KB cache
"fram")
    module load intel/2020a
    export CC=icc
    export CFLAGS+="-qopenmp -DSTREAM_ARRAY_SIZE=80000000 "
    ;;
# AMD Epyc 7742, 64 cores, 256000 KB cache
"betzy")
    module load intel/2020a
    export CC=icc
    export CFLAGS+="-qopenmp -DSTREAM_ARRAY_SIZE=300000000 -mcmodel=medium "
    ;;
*)
    echo "[ERROR] Invalid label"
esac
