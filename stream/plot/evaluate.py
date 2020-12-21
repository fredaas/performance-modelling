#!/usr/bin/env python3


from glob import glob

def max(a, b):
    return a if a > b else b

def read_file(path):
    file = open(path, "r")
    file.readline()
    content = file.read().rstrip().split("\n")
    file.close()
    return content

for path in glob("./*scatter.txt", recursive=False):
    file = read_file(path)
    max_bandwidth = 0.0
    for line in file:
        line = line.split(" ")
        max_bandwidth = max(float(line[2]), max_bandwidth)
    print(path, max_bandwidth, "GB/s");
