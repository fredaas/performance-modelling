#!/usr/bin/env bash

# Compact
mpiexec -n 4 --report-bindings --bind-to core --map-by core ./main
# Scatter
mpiexec -n 4 --report-bindings --bind-to core --map-by socket ./main
