#!/usr/bin/env python

import random
import sys
import numpy

FROM = -100
TO = 100

def main():

    dim_rows = int(sys.argv[1])
    dim_cols = int(sys.argv[2])
    work_items = int(sys.argv[3])

    f = open("mat1", "w")
    f.write("{0}\n".format(dim_rows))
    mtx1 = write_matrix(f,dim_rows, work_items)
    f.close()

    f = open("mat2", "w")
    f.write("{0}\n".format(dim_cols))
    mtx2 = write_matrix(f, work_items, dim_cols)
    f.close()

    sys.stdout.write("{0}:{1}\n".format(dim_rows, dim_cols))
    mtx_res = mtx1 * mtx2
    for i in range(dim_rows):
        for j in range(dim_cols):
            if j != 0:
                sys.stdout.write(" ")
            sys.stdout.write("{0}".format(int(mtx_res[i,j])))
        sys.stdout.write("\n")



def write_matrix(f, rows, cols):
    mtx = numpy.matrix(numpy.empty((rows,cols,)))
    for i in range(rows):
        for j in range(cols):
            if j != 0:
                f.write(" ")
            number = random.randint(FROM, TO)
            mtx[i,j] = number
            f.write("{0}".format(number))
        if i != rows -1:
            f.write('\n')
    return mtx

if __name__=="__main__":
    main()
