#!/usr/bin/env bash

printf "\nDELL XPS: Latency\n"
./compute_metrics.py "./data/dellxps_alpha.txt" 1 4
printf "\nDELL XPS: Inverse bandwidth\n"
./compute_metrics.py "./data/dellxps_beta.txt" 1 4
