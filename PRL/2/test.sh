#!/bin/bash

if [ $# -eq 1 ];then
    numbers=$1;
else
    echo "Chybne parametry."
    exit 1
fi;

#Merlin
mpic++ --prefix /usr/local/share/OpenMPI -o es es.cpp

#vyrobeni souboru s random cisly
( dd if=/dev/urandom bs=1 count=$numbers of=numbers ) > /dev/null 2>&1

#Merlin
processors=$((numbers+1));
mpirun --prefix /usr/local/share/OpenMPI -np $processors es

#uklid
rm -f numbers es
