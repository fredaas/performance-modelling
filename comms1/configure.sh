#!/usr/bin/env bash

CFLAGS="-std=c99 -Wall -O2 "

case $1 in
"dellxps")
    export CC=mpicc
    ;;
"idun")
    module load intel/2019a
    export CC=mpiicc
    export CFLAGS+="-lmpi "
    ;;
"fram")
    module load intel/2019a
    export CC=mpiicc
    export CFLAGS+="-lmpi "
    ;;
*)
    echo "Invalid label"
    ;;
esac
