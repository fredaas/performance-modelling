#!/usr/bin/env bash

system=$1

CFLAGS="-std=c99 -Wall -O2 -I./ -Wno-unused-but-set-variable "

case $1 in
# Intel Core i7-7700HQ
"dellxps")
    export L3_SIZE=6144 # KB
    export CC=gcc
    export CFLAGS+="-fopenmp "
    ;;
# Intel Xeon E5-2630 v2
"idun-e5")
    module load intel
    export L3_SIZE=25000 # KB
    export CC=icc
    export CFLAGS+="-qopenmp -DSTREAM_ARRAY_SIZE=80000000 "
    ;;
# Intel Xeon Gold 6132
"idun-gold")
    module load intel
    export L3_SIZE=19000 # KB
    export CC=icc
    export CFLAGS+="-qopenmp -DSTREAM_ARRAY_SIZE=80000000 "
    ;;
# Intel Xeon E5-2683 v4
"fram-e5")
    module load intel/2020a
    export L3_SIZE=40000 # KB
    export CC=icc
    export CFLAGS+="-qopenmp -DSTREAM_ARRAY_SIZE=80000000 "
    ;;
*)
    echo "[ERROR] Invalid label"
esac
