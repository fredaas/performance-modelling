#!/usr/bin/env python3

def fread(path):
    fp = open(path, "r")
    content = fp.read().split("\n")
    content = [ x for x in content if len(x) > 0 ]
    fp.close()
    return content

def fwrite(path, matrix):
    fp = open(path, "w")
    for row in matrix:
        for cell in row:
            fp.write(str(cell) + " ")
        fp.write("\n")
    fp.close()


def convert(path):
    matrix = fread(path)

    rows = len(matrix)
    cols = len(matrix[0].rstrip().split(" "))

    cmatrix = []

    for i, rows in enumerate(matrix):
        cols = rows.rstrip().split(" ")
        cmatrix.append([])
        for cell in cols:
            cell = float(cell)
            if (cell > 1.0E-20):
                cmatrix[i].append(1.0E+9 / cell)
            else:
                cmatrix[i].append(0.0)

    fwrite("converted_" + path, cmatrix)
