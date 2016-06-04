#!/bin/sh
# Skript le-diff.sh pro 1. projekt do predmetu IOS
# Vojtech Havlena, xhavle03@stud.fit.vutbr.cz

#Funkce, ktera zjisti, zda soubor je v ignore
Ignore() {
   echo "$ignoreInput" | while read -r ignore ; do
      test "$ignore" = "" && continue
      echo "$file" | grep -E -- "$ignore" > /dev/null && {
         echo "false"
         return 0
      }
   done
}
if [ ! -r ".le/.config" ] ; then
   echo "Soubor .le/.config neni pro cteni" >&2
   exit 1;
fi
projdir=`grep -E '^projdir ' ".le/.config" | sed -r 's/^projdir //' | head -n1`
if [ ! -d "$projdir" ] ; then
   echo "Adresar projektu neexistuje" >&2
   exit 1
fi
if [ $# -eq 0 ] ; then #Zpracovani parametru
   query="`find "$projdir" ./ -maxdepth 1 -type f \( ! -iname '.*' \) | sed -r 's/.*\///' | sort | uniq`"
else
   for param in "$@" ; do 
      echo "$param" | grep -E '(^|\/)\.[^\/]+$' > /dev/null || query=`printf "%s\n%s" "$query" "$param"`
   done
   query=`echo "$query"`
fi
ignoreInput=`cat .le/.config | grep -E '^ignore ' | cut -c8-` #sed -r 's/^ignore //'`
ret=0

IFS=''
echo "$query" | { 
while read -r file ; do
   skip=`Ignore .le/.config`
   test "$skip" = "false" && continue
   test "$file" = "" && continue
   
   if [ ! -f "$projdir"/"$file" ] && [ ! -f ./"$file" ] ; then
      echo Soubor "$file" neexistuje. >&2
      ret=1
      continue
   fi
   if [ ! -f "$projdir"/"$file" ] ; then
      echo D: "$file"
      continue
   fi
   if [ ! -f ./"$file" ] ; then
      echo C: "$file"
      continue
   fi
   diff -u -- "$projdir"/"$file" ./"$file" && echo .: "$file"
   if [ $? -eq 2 ] ; then 
      ret=1
   fi
done 
exit $ret
}
