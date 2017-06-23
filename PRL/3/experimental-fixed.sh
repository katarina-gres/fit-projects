#!/bin/bash

experiments=20;
declare -a proc=(200)

#: > "Experimenty/exp.data"

for i in "${proc[@]}"; do
   echo "$i"
   tname="Experimenty/Fixed/${i}.time";
   : > $tname
   for((k=0; k<$experiments; k++)); do
      python generator.py 5 5 $i > Tmp/res1.out
      bash test.sh > Tmp/tmp.out
      cat Tmp/tmp.out | head -n 1 >> $tname
      cat Tmp/tmp.out | tail -n +2 > Tmp/res2.out

      difference=$(diff Tmp/res1.out Tmp/res2.out)
      if [ "$difference" != "" ]; then
         echo "Error"
      fi
   done
   echo -n "${i} " >> "Experimenty/Fixed/exp.data"
   sort -k1 -r ${tname} | awk -F : '{sum+=$1} END {print sum/NR}' >> "Experimenty/Fixed/exp.data"
done
