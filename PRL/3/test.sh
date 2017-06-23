#!/bin/bash

# if [ ! -f "mat1" ]; then
#    echo "Soubor mat1 neexistuje."
#    exit 1
# fi
#
# if [ ! -f "mat2" ]; then
#    echo "Soubor mat2 neexistuje."
#    exit 1
# fi

mat1=$(head -n1 mat1)
mat2=$(head -n1 mat2)

# if [[ ! $mat1 =~ ^[[:space:]]*[0-9]+[[:space:]]*$ ]]; then
#    echo "Soubor mat1 musi mit na prvnim radku rozmer."
#    exit 1
# fi
#
# if [[ ! $mat2 =~ ^[[:space:]]*[0-9]+[[:space:]]*$ ]]; then
#    echo "Soubor mat2 musi mit na prvnim radku rozmer."
#    exit 1
# fi

cpus=$((mat1*mat2))

mpic++ --prefix /usr/local/share/OpenMPI -o mm mm.cpp -std=c++0x
mpirun --prefix /usr/local/share/OpenMPI -np $cpus mm
rm -f mm
