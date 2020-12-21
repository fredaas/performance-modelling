#!/usr/bin/env python3

import os
import sys

def on_same_socket(i, j, ncores, affinity):
    if affinity == "close":
        n = ncores / 2
        return int(i / n) == int(j / n)

def fread(path):
    fp = open(path, "r")
    content = fp.read().split("\n")
    content = [ x for x in content if len(x) > 0 ]
    fp.close()
    return content

def compute_metrics(path, nsockets, ncores, affinity):
    """
PARAMETERS

    path

        Target file

    nsockets

        # sockets per node

    ncores

        # cores per node

    affinity

        'spread' or 'close'
    """
    matrix = fread(path)

    min_intra_socket   = float("inf")
    max_intra_socket   = 0.0
    min_inter_socket   = float("inf")
    max_inter_socket   = 0.0
    min_inter_node     = float("inf")
    max_inter_node     = 0.0
    avg_inter_node     = 0.0
    avg_intra_socket   = 0.0
    avg_inter_socket   = 0.0

    count_intra_socket = 0
    count_inter_socket = 0
    count_inter_node   = 0

    for rank_i, row in enumerate(matrix):
        cols = row.strip().split(" ")
        start = rank_i + 1
        node_i = int(rank_i / ncores)
        for rank_j, col in enumerate(cols[start:], start=start):
            node_j = int(rank_j / ncores)
            col = float(col)
            # Rank i and j are on separate nodes
            if node_j > node_i:
                if col < min_inter_node:
                    min_inter_node = col
                if col > max_inter_node:
                    max_inter_node = col
                avg_inter_node += col
                count_inter_node += 1
            # Rank i and j are on the same node
            else:
                if nsockets == 1:
                    if col < min_intra_socket:
                        min_intra_socket = col
                    if col > max_intra_socket:
                        max_intra_socket = col
                    avg_intra_socket += col
                    count_intra_socket += 1
                else:
                    # Rank i and j are on the same socket
                    if on_same_socket(rank_i, rank_j, ncores, affinity):
                        if col < min_intra_socket:
                            min_intra_socket = col
                        if col > max_intra_socket:
                            max_intra_socket = col
                        avg_intra_socket += col
                        count_intra_socket += 1
                    else:
                        if col < min_inter_socket:
                            min_inter_socket = col
                        if col > max_inter_socket:
                            max_inter_socket = col
                        avg_inter_socket += col
                        count_inter_socket += 1

    if count_intra_socket > 0:
        avg_intra_socket /= count_intra_socket
    if count_inter_socket > 0:
        avg_inter_socket /= count_inter_socket
    if count_inter_node > 0:
        avg_inter_node /= count_inter_node

    rows = len(matrix)
    cols = len(matrix[0].strip().split(" "))

    print("    {} x {}".format(rows, cols))

    print("    Intra-socket (min/max/avg): {:.3} / {:.3} / {:.3}".format(
        min_intra_socket,
        max_intra_socket,
        avg_intra_socket))

    print("    Inter-socket (min/max/avg): {:.3} / {:.3} / {:.3}".format(
        min_inter_socket,
        max_inter_socket,
        avg_inter_socket))

    print("    Inter-node (min/max/avg):   {:.3} / {:.3} / {:.3}".format(
        min_inter_node,
        max_inter_node,
        avg_inter_node))

print("Fram-E5")
print("    ALPHA")
compute_metrics("fram_e5_alpha128.txt", 2, 32, "close")
print("    BETA")
compute_metrics("fram_e5_beta128.txt", 2, 32, "close")

print("Idun-E5")
print("    ALPHA")
compute_metrics("idun_e5_alpha120.txt", 2, 20, "close")
print("    BETA")
compute_metrics("idun_e5_beta120.txt", 2, 20, "close")

print("Idun-Gold")
print("    ALPHA")
compute_metrics("idun_gold_alpha112.txt", 2, 28, "close")
print("    BETA")
compute_metrics("idun_gold_beta112.txt", 2, 28, "close")
