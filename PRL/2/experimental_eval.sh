#!/bin/bash

experiments=50;
runs=1;
declare -a proc=(10)
#declare -a proc=(5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30)
#declare -a proc=(36 35)

: > "Experimenty/exp.data"

for i in "${proc[@]}"; do
   echo "$i"
   for((k=0; k<$runs; k++)); do
      tname="Experimenty/${i}.time";
      tmp="Experimenty/${i}.tmp";
      avgname="Experimenty/${i}.avg";
      : > $tmp
      for ((j=1; j<=$experiments; j++)); do
         ( dd if=/dev/random bs=1 count=$i of=numbers ) > /dev/null 2>&1
         bash test $i > tmp0
         cat tmp0 | head -n 1 | tr " " "\n" | sort -n > tmp1
         cat tmp0 | tail -n +2 > tmp2
         difference=$(diff tmp1 tmp2)
         if [ "$difference" != "" ]; then
            echo "Error"
            cat tmp0
         fi
         sed '2q;d' tmp0 >> $tmp
         rm tmp0 tmp1 tmp2
      done
      rm -f numbers
      sort -k1 -r ${tmp} | tail -n +1 | head -n -1 | awk -F : '{sum+=$1} END {print sum/NR}' >> $tname    
   done
   echo -n "${i} " >> "Experimenty/exp.data"
   sort -k1 -r ${tname} | awk -F : '{sum+=$1} END {print sum/NR}' >> "Experimenty/exp.data"
done
