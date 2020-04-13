#!/usr/bin/env bash

case $1 in
"idun")
    # mpiexec_mpt
    export CC=icc
    export CFLAGS="-std=c99 -Wall -O2 -lmpi"
    ;;
"vilje")
    # mpiexec_mpt
    export CC=icc
    export CFLAGS="-std=c99 -Wall -O2 -lmpi"
    ;;
"local")
    # mpirun
    export CC=mpicc
    export CFLAGS="-std=c99 -Wall -O2"
    ;;
*)
    echo "[ERROR] Invalid system, no flags set"
    return 1;
esac
