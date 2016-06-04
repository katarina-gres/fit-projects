#!/bin/sh
# Skript le-checkout.sh pro 1. projekt do predmetu IOS
# Vojtech Havlena, xhavle03@stud.fit.vutbr.cz

#Funkce, ktera zjisti, zda soubor je v ignore
Ignore() {
   ignoreInput=`cat "$1" | grep -E '^ignore ' | cut -c8-` #sed -r 's/^ignore //'`
   echo "$ignoreInput" | while read -r ignore ; do
      v=`echo "$file" | grep -E -- "$ignore"`
      if [ "$v" != "" ] && [ "$ignore" != "" ] ; then
         echo "false"
         return 0;
      fi
   done
}
ret=0
if [ ! $# -eq 1 ] ; then
   echo "Chybne parametry" >&2
   exit 1
fi
configFile=".le/.config"
if [ ! -d "$1" ] ; then
   echo "Adresar $1 neexistuje" >&2
   exit 1
fi
if [ -d ".le" ] ; then
   find ".le" -maxdepth 1 -mindepth 1 -type f \( ! -iname '.*' \) -print0 | xargs -0 rm -f #smazani souboru
else
   mkdir ".le"
fi
if [ -f ".le/.config" ] ; then
   if [ ! -w ".le/.config" ] ; then
      echo "Soubor .le/.config neni urcen pro zapis" >&2
      exit 1 
   fi
   newConfig=`cat $configFile | sed -r "/^projdir /d"`
   echo "$newConfig" > "$configFile"
fi
echo "projdir $1" >> "$configFile"
actualDir=`pwd`
cd "$1" || exit 1
IFS=''
for file in * ; do
   if [ ! -f "$file" ]; then
      continue
   fi
   copy=`Ignore "$actualDir"/$configFile`
   if [ "$copy" != "false" ] ; then
      cp -- ./"$file" "$actualDir"/.le/"$file" || ret=1
      cp -- ./"$file" "$actualDir"/"$file" || ret=1
   fi
done
exit $ret
