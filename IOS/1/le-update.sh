#!/bin/sh
# Skript le-update.sh pro 1. projekt do predmetu IOS
# Vojtech Havlena, xhavle03@stud.fit.vutbr.cz

#Funkce, ktera zjisti, zda soubor je v ignore
Ignore() {
   ignoreInput="`cat "$1" | grep -E '^ignore ' | cut -c8-`"
   echo "$ignoreInput" | while read -r ignore ; do
      v=`echo "$file" | grep -E -- "$ignore"`
      if [ "$v" != "" ] && [ "$ignore" != "" ] ; then
         echo "false" #Soubor bude ignorovan
         return 0
      fi
   done
}

if [ ! -r ".le/.config" ] ; then
   echo "Soubor .le/.config neni pro cteni" >&2
   exit 1;
fi
#Zjistim projdir z .configu
projdir=`grep -E '^projdir ' ".le/.config" | sed -r 's/^projdir //' | head -n1`
if [ ! -d "$projdir" ] ; then
   echo "Adresar projektu neexistuje" >&2
   exit 1
fi
#Zpracovani parametru
if [ $# -eq 0 ] ; then
   query="`find "$projdir" ./ ./.le -maxdepth 1 -type f \( ! -iname '.*' \) | sed -r 's/.*\///' | sort | uniq`"
else
   for param in "$@" ; do 
      echo "$param" | grep -E '(^|\/)\.[^\/]+$' > /dev/null || query=`printf "%s\n%s" "$query" "$param"`
   done
   query=`echo "$query"`
fi

tmpPath="/tmp/projIOS"
mkdir "$tmpPath" 2> /dev/null

ignoreInput="`cat .le/.config | grep -E '^ignore ' | cut -c8-`" #sed -r 's/^ignore //'`
ret=0
IFS=''
echo "$query" | {
while read -r file ; do
   skip=`Ignore .le/.config`
   test "$skip" = "false" && continue
   
   if [ ! -f "$projdir"/"$file" ] && [ -f .le/"$file" ] ; then
      echo D: "$file"
      rm .le/"$file" ./"$file" > /dev/null
      continue
   fi
   if [ ! -f ./"$file" ] && [ -f "$projdir"/"$file" ] ; then
      echo C: "$file"
      cp -- "$projdir"/"$file" .le/"$file" || ret=1
      cp -- "$projdir"/"$file" ./"$file" || ret=1
      continue
   fi
   if [ ! -f "$file" ] || [ ! -f "$projdir"/"$file" ] || [ ! -f .le/"$file" ] ; then
      continue
   fi

   if diff -u -- ./"$file" "$projdir"/"$file" > /dev/null ; then
      if diff -u ./"$file" .le/"$file" > /dev/null ; then        #soubory jsou stejne
         echo .: "$file"
         continue
      else
         echo UM: "$file"
         cp -- "$projdir"/"$file" .le/"$file" || ret=1
         continue
      fi
   elif diff -u -- .le/"$file" "$projdir"/"$file" > /dev/null ; then
      echo M: "$file"
      continue
   elif diff -u .le/"$file" ./"$file" > /dev/null ; then
      echo U: "$file"
      cp -- "$projdir"/"$file" .le/"$file"
      cp -- "$projdir"/"$file" ./"$file"
   else
      #lisi se ve vsech umistenich
      cp -- ./"$file" "$tmpPath"/"$file" || ret=1 #kopie souboru z exp
      diff -u -- .le/"$file" "$projdir"/"$file" > "$tmpPath"/"${file}.patch"
      if patch -fs -d "$tmpPath" < "$tmpPath"/"${file}.patch" > /dev/null 2> /dev/null ; then
         echo M+: "$file"
         cp -- "$projdir"/"$file" .le/"$file" || ret=1
         cp -- "$tmpPath"/"$file" "$file" || ret=1
      else
         echo M!: "$file" conflict!  
      fi
   fi
done
rm "$tmpPath"/* 2> /dev/null
rmdir "$tmpPath" 2> /dev/null
exit $ret
}

