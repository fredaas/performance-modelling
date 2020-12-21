#!/usr/bin/env python3

from glob import glob

def min(a, b):
    return a if a < b else b

def max(a, b):
    return a if a > b else b

def fread(path):
    fp = open(path, "r")
    fp.readline()
    rows = fp.read().rstrip().split("\n")
    fp.close()
    return rows

def compute_metrics(path):
    rows = fread(path)
    max_flops = 0
    for x in rows:
        x = x.split(" ")
        max_flops = max(max_flops, float(x[2]))
    print(max_flops, end="\n\n")

print("FRAM-E5", end=" ")
compute_metrics("fram_e5.txt")

print("IDUN-E5", end=" ")
compute_metrics("idun_e5.txt")

print("IDUN-GOLD", end=" ")
compute_metrics("idun_gold.txt")
