#!/bin/bash

experiments=10;
declare -a proc=(9)

#: > "Experimenty/exp.data"

for i in "${proc[@]}"; do
   echo "$i"
   tname="Experimenty/Square/${i}.time";
   : > $tname
   for((k=0; k<$experiments; k++)); do
      python generator.py $i $i $i > Tmp/res1.out
      bash test.sh > Tmp/tmp.out
      cat Tmp/tmp.out | head -n 1 >> $tname
      cat Tmp/tmp.out | tail -n +2 > Tmp/res2.out

      difference=$(diff Tmp/res1.out Tmp/res2.out)
      if [ "$difference" != "" ]; then
         echo "Error"
      fi
   done
   echo -n "${i} " >> "Experimenty/Square/exp.data"
   sort -k1 -r ${tname} | awk -F : '{sum+=$1} END {print sum/NR}' >> "Experimenty/Square/exp.data"
done
