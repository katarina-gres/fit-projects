#!/bin/sh
# Skript le-revert.sh pro 1. projekt do predmetu IOS
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
configFile=".config"
if [ ! -r ".le/.config" ] ; then
   echo "Soubor .le/.config neni pro cteni" >&2
   exit 1;
fi
cd ".le" || exit 1
if [ $# -eq 0 ] ; then #Zpracovani parametru
   query="`find ./ -maxdepth 1 -type f \( ! -iname '.*' \)`"
else
   for param in "$@" ; do 
      echo "$param" | grep -E '(^|\/)\.[^\/]+$' > /dev/null || query=`printf "%s\n%s" "$query" "$param"`
   done
   query=`echo "$query"`
fi
ret=0
ignoreInput=`cat $configFile | grep -E '^ignore ' | cut -c8-` #sed -r 's/^ignore //'`
IFS=''
echo "$query" | {
while read -r file ; do
   test "$file" = "" && continue
   if [ ! -f "$file" ]; then
      echo "Soubor $file neexistuje." >&2
      ret=1 
      continue
   fi
   copy=`Ignore`
   if [ "$copy" != "false" ] ; then
      cp ./"$file" ../"$file" || ret=1
   fi
done   
exit $ret
}
