#!/bin/bash
## autor: Vitezslav Kriz (xkrizv01)


MAIN=../../../ifj13

STDOUT=stdout.output
STDERR=stderr.output
PHPOUT=phpout.output
PHPERR=phperr.output
TMP=tmpfile.php
VALLOG=valgrind.output
LASTFILELOG=lasttest.log

BONUS=0

INTER=0
VALGR=0
CONT=0
cnt_cont=0

while getopts ":vicxf:s:" opt; do
  case $opt in
    i)
	  echo "Interactive mode"
      INTER=1
      ;;
    x)
	  echo "Autostop mode"
      INTER=2
      ;;
    v)
	  echo "Valgrind mode"
      VALGR=1
      ;;      
    c)
	  if [ -f $LASTFILELOG ]; then
		  CONT=$(cat $LASTFILELOG)
		  echo "Continue from: $CONT"
      fi
      ;;
    s)
	  CONT="$OPTARG"
	  echo "Continue from: $CONT"
      ;;    
    f)
	  MAIN=$OPTARG
      ;;      
    \?)
      echo "Invalid option: -$OPTARG" >&2
      echo "usage:"
      echo "./test.sh -i\tInteracitve mode"
      echo "./test.sh -x\tAutostop mode"
      echo "./test.sh -v\tTest with valgrind"
      ;;
  esac
done



rm -f empty
touch empty

if [ -t 1 ]; then
    c_red=`tput setaf 1`
    c_green=`tput setaf 2`
    c_normal=`tput sgr0`
fi

E_OK=0
E_LEX=1
E_SYN=2
E_DEFFUNC=3
E_MISPAR=4
E_UNDEF=5
E_DIVZERO=10
E_DVAL=11
E_TYPE_COMP=12
E_OTHER=13
ecodes[0]=E_OK
ecodes[1]=E_LEX
ecodes[2]=E_SYN
ecodes[3]=E_DEFFUNC
ecodes[4]=E_MISPAR
ecodes[5]=E_UNDEF
ecodes[10]=E_DIVZERO
ecodes[11]=E_DOUBLEVAL
ecodes[12]=E_TYPE_COMP
ecodes[13]=E_OTHER_SEMANTIC
ecodes[139]="${c_red}SEGMENTATION FAULT$c_normal"



cnt=0
passed=0
failed=0
fail=0

# params source code, stdin,
function valgrindtest(){
if [ $VALGR -eq 1 ]; then
  valgrind --leak-check=full --show-reachable=yes --error-exitcode=42 --log-file=$VALLOG $MAIN $1 < $2 > /dev/null 2>/dev/null
  rcode=$?
  if [ $rcode -ne 42 ]; then
    printf "VG:${c_green}pass$c_normal\t"
  else
    printf "VG:${c_red}FAIL$c_normal\t"
    fail=$[$fail+4];
  fi
fi
}

#params source_code,stdin, stdout, return_code, comment
function test1(){
 if [ $CONT -ne 0 ]; then
   if [ $CONT -eq $cnt_cont ]; then
     CONT=0;
   else
     cnt_cont=$[$cnt_cont+1]
     return
   fi
 fi
 
 if ! [ -f $1 ]; then
   echo "${c_red}Mising test file: $1 $c_normal"
   return
 fi
 if ! [ -f $2 ]; then
   echo "${c_red}Mising test file: $2 $c_normal"
   return
 fi
 if ! [ -f $3 ]; then
   echo "${c_red}Mising test file: $3 $c_normal"
   return
 fi
 $MAIN $1  < $2 > $STDOUT 2> $STDERR
 rcode=$?
 cnt=$[$cnt+1]
 fail=0;
 if [ $rcode -eq $4 ]; then
 	printf "RC:${c_green}pass$c_normal\t"
 else
 	printf "RC:${c_red}FAIL$c_normal\t"
 	fail=$[$fail+1];
 fi

 if diff -q $STDOUT $3 > /dev/null; then
 	printf "OUT:${c_green}pass$c_normal  "
 else
 	printf "OUT:${c_red}FAIL$c_normal  "
 	fail=$[$fail+2];
 fi
 
 valgrindtest $1 $2
 
 printf "%s \n" "$5"

# if [ $4 -eq $E_OK ]; then
#	 if diff -q $STDERR empty; then
#	 	printf "${c_red}stderr not empty$c_normal  "
#	 fi
# fi

 if [ $fail -ne 0 ]; then
 	failed=$[$failed+1]
 else
 	passed=$[$passed+1]
 fi

 if [[ $fail == 1 || $fail == 3 ]]; then
 	echo "  expected: ${4}:${ecodes[$4]}, return: ${rcode}:${ecodes[$rcode]}"
 fi

 if [ $fail -ne 0 ]; then
   if [ $INTER -eq 1 ]; then
     echo "Prerusit test (zachova vystupni soubory) [a/n]"
     read xyz
     if [[ $xyz == "a" || $xyz == "A" || $xyz == "y" || $xyz == "Y" ]]; then
       printf "test prerusen\n\tskript: $1\n\tvstup: $2\n\tocekavany vystup: $3\n\tstdout: $STDOUT\n\tstderr: $STDERR\n\tvalgrind: $VALLOG\n"
       printf "$cnt_cont" > $LASTFILELOG
       exit 1
     fi
   fi
   if [ $INTER -eq 2 ]; then
     echo "${c_red}TEST PRERUSEN$c_normal"
     printf "test prerusen\n\tskript: $1\n\tvstup: $2\n\tocekavany vystup: $3\n\tstdout: $STDOUT\n\tstderr: $STDERR\n\tvalgrind: $VALLOG\n"
     printf "$cnt_cont" > $LASTFILELOG
     exit 1
   fi
 fi

  cnt_cont=$[$cnt_cont+1]
}

#params source_code,stdin, comment
function testphp(){
 if [ $CONT -ne 0 ]; then
   if [ $CONT -eq $cnt_cont ]; then
     CONT=0;
   else
     cnt_cont=$[$cnt_cont+1]
     return
   fi
 fi
 if ! [ -f $1 ]; then
   echo "${c_red}Mising test file: $1 $c_normal"
   return
 fi
 if ! [ -f $2 ]; then
   echo "${c_red}Mising test file: $2 $c_normal"
   return
 fi
 $MAIN $1  < $2 > $STDOUT 2> $STDERR
 rcode=$?
 cnt=$[$cnt+1]
 fail=0;
 if [ $rcode -eq 0 ]; then
 	printf "RC:${c_green}pass$c_normal\t"
 else
 	printf "RC:${c_red}FAIL$c_normal\t"
 	fail=$[$fail+1];
 fi
 php -d open_basedir="" ifj13.php $1 < $2 > $PHPOUT 2> $PHPERR

 if ! diff -q $PHPERR empty; then
 	printf " Spatny testovaci skript. Musi projit php."
 	exit
 fi

 if diff -q $STDOUT $PHPOUT > /dev/null; then
 	printf "OUT:${c_green}pass$c_normal  "
 else
 	printf "OUT:${c_red}FAIL$c_normal  "
 	fail=$[$fail+2];
 fi
 
 valgrindtest $1 $2
 
 printf "%s \n" "$3"



 if [ $fail -ne 0 ]; then
 	failed=$[$failed+1]
 else
 	passed=$[$passed+1]
 fi

 if [[ $fail == 1 || $fail == 3 ]]; then
 	echo "  expected: ${E_OK}:${ecodes[$E_OK]}, return: ${rcode}:${ecodes[$rcode]}"
 fi

 if [ $fail -ne 0 ]; then
   if [ $INTER -eq 1 ]; then
     echo "Prerusit test (zachova vystupni soubory) [a/n]"
     read xyz
     if [[ $xyz == "a" || $xyz == "A" || $xyz == "y" || $xyz == "Y" ]]; then
       printf "test prerusen\n\tskript: $1\n\tvstup: $2\n\tocekavany vystup: $PHPOUT\n\tstdout: $STDOUT\n\tstderr: $STDERR\n\tvalgrind: $VALLOG\n"
       printf "$cnt_cont" > $LASTFILELOG
       exit 1
     fi
   fi
   if [ $INTER -eq 2 ]; then
     echo "${c_red}TEST PRERUSEN$c_normal"
     printf "test prerusen\n\tskript: $1\n\tvstup: $2\n\tocekavany vystup: $PHPOUT\n\tstdout: $STDOUT\n\tstderr: $STDERR\n\tvalgrind: $VALLOG\n"
     printf "$cnt_cont" > $LASTFILELOG
     exit 1
   fi
 fi
 
 cnt_cont=$[$cnt_cont+1]
}
#params return code, inline source code
function test1a(){
  echo "$2" > $TMP
  test1 "$TMP" "empty" "empty" $1 "code: $2"
}

#params  inline source code
function testphpa(){
  echo "$1" > $TMP
  testphp "$TMP" "empty" $1
}


################################################################################

#  zdrojovy kod, vstup, ocekavany vystup, navratovy kod, zprava - komentar testu
test1 "helloworld.php" "empty" "helloworld.out" $E_OK "Hello world test"

test1 "all/zadani_fact.php" "all/zadani_fact.in" "all/zadani_fact.out" $E_OK "Zadani IFJ13: factorial"
test1 "all/zadani_vestavene.php" "all/zadani_vestavene.in" "all/zadani_vestavene.out" $E_OK "Zadani IFJ13: vestavene fce"



test1 "basic/putget1.php" "basic/putget1.out" "basic/putget1.out" $E_OK "get_string, put_string"
test1 "basic/dollar1.php" "empty" "empty" $E_LEX "<?php \$x=put_string("\"\$ahoj\"");"
test1 "basic/undef1.php" "empty" "empty" $E_UNDEF "<?php \$x = \$x"
test1 "basic/undef2.php" "empty" "empty" $E_UNDEF "<?php \$x = put_string(\$x)"
#test1 "basic/doubleval1.php" "empty" "empty" $E_DVAL "Pretypovaani 3.1e"
test1 "basic/vyraz1.php" "empty" "empty" $E_OK "Jiny typ nez bool v ifu"
test1 "basic/doubleval.php" "empty" "basic/zero" $E_OK "Doubleval"
test1a $E_OK "<?php return null;  "
test1a $E_DVAL "<?php \$x=doubleval(\"3.1e\");"
test1a $E_DVAL "<?php \$x=doubleval(\"   3.\");"
test1a $E_SYN " <?php"
test1a $E_SYN "<?php\$x=5;"
test1 "basic/param1.php" empty empty $E_OTHER "Shodne parametry v definici funkce"
#test1 "basic/strlen.php" "empty" "basic/strlen.out" $E_OK "Strlen test"

test1a $E_TYPE_COMP "<?php \$x= null + null;"
test1a $E_TYPE_COMP "<?php \$x= 1 + null;"
test1a $E_TYPE_COMP "<?php \$x= 1 + \"ahoj\";"
test1a $E_TYPE_COMP "<?php \$x= 1 . \"ahoj\";"
test1a $E_OK "<?php \$x= \"ahoj\" . 1;"


test1 "basic/escape1.php" empty "basic/escape1.out" $E_OK "Chyba escape sekvence - nevyrobi chybu"
test1 "basic/lexinfunc.php" empty empty $E_LEX "Lexikalni chyba pri definici funkce n.1"
test1 "basic/lexinfunc1.php" empty empty $E_LEX "Lexikalni chyba pri definici funkce n.2"
test1 "basic/lexinfunc2.php" empty empty $E_LEX "Lexikalni chyba pri definici funkce n.3"
test1 "basic/lexinfunc3.php" empty empty $E_LEX "Lexikalni chyba pri volani funkce n.1"
test1 "basic/lexinfunc4.php" empty empty $E_LEX "Lexikalni chyba pri volani funkce n.2"
test1 "basic/lexinfunc5.php" empty empty $E_LEX "Lexikalni chyba pri volani funkce n.3 - pripadne ve vyrazu"
test1 "basic/comment1.php" empty empty $E_LEX "Neukonceny blokovy komentar - lex. chyba"
test1 "basic/comment2.php" empty empty $E_OK "komentar na radku s definici funkce"
test1 "basic/redefpar1.php" empty empty $E_OTHER "Redefinice parametru funkce"
test1 "basic/strlit1.php" empty empty $E_LEX "V retezcovem literalu je enter"
test1 "basic/strlit2.php" empty empty $E_LEX "V retezcovem literalu je tabulator"
test1 "basic/intval1.php" empty empty $E_OK "V celocis. lit. mohou byt pouze cisla. intval(\"+31\")===0"
test1 "basic/divzero.php" empty empty $E_DIVZERO "Deleni nulou"
test1 "basic/return1.php" empty empty $E_OK "Navratova hodnota #0"
test1 "basic/mispar1.php" empty empty $E_MISPAR "Chybejici parametr funkce"
test1 "basic/casesensitive.php" empty empty $E_DEFFUNC "Case sensitive test"
test1 "basic/promenespravne.php" empty empty $E_OK "Nazvy promenych spravne"
test1 "basic/promenespatne.php" empty empty $E_LEX "Nazvy promenych spatne"
test1 "basic/typecomp.php" empty empty $E_TYPE_COMP "Typova kompatabilita"
test1 "basic/deffunc.php" empty empty $E_SYN "Definice funkce nesmi byt ve funkci"
test1 "basic/deffunc.php" empty empty $E_SYN "Definice funkce nesmi byt ve if bloku"
test1 "basic/lokalnostfalse.php" empty empty $E_UNDEF "Lokalnost promenych false"
test1 "basic/radkovykomentar.php" empty empty $E_OK "Radkovy komentar neukonecny enterem"

test1 "basic/zavorky1.php" empty empty $E_SYN "(+1-1) - neplati pro unarni plus"




echo "************ test redefinice funkci ************"
# navratovy kod, inline zdrojovy kod
# vstup i vystup je prazdny
test1a $E_DEFFUNC "<?php function boolval(){}"
test1a $E_DEFFUNC "<?php function doubleval(){}"
test1a $E_DEFFUNC "<?php function intval(){}"
test1a $E_DEFFUNC "<?php function strval(){}"
test1a $E_DEFFUNC "<?php function get_string(){}"
test1a $E_DEFFUNC "<?php function put_string(){}"
test1a $E_DEFFUNC "<?php function strlen(){}"
test1a $E_DEFFUNC "<?php function get_substring(){}"
test1a $E_DEFFUNC "<?php function find_string(){}"
test1a $E_DEFFUNC "<?php function sort_string(){}"
test1a $E_DEFFUNC "<?php function hello(){} function hello(){}"








# test oproti php. Urceno jen pro spravne napsane programy v php
#        zdrojovy kod   ,vstup, zprava
testphp "helloworld.php" "empty" "Hello world test"
testphp "basic/putget2.php" "poezie" "Prepisovani vstupu na vystup"
testphp "basic/xval1.php" "empty" "Test pretypovani"
testphp "basic/sort_string2.php" "empty" "Test sort_string"
testphp "basic/fact.php" "empty" "Faktorial rekurzivne"
testphp "basic/findstr.php" "empty" "find_string()"
testphp "basic/aritmetika1.php" "empty" "aritmeticke operace"
testphp "basic/null1.php" "empty" "porovnavani null"
testphp "elseif/compare1.php" "elseif/compare1.in" "Test porovnavani stringu"
test1 "basic/compare2.php" "empty" "basic/compare2.out" $E_OK "Porovnavani null false a true"
test1a $E_TYPE_COMP "<?php \$x= true + 5;"
test1 "basic/compare2.php" "empty" "basic/compare2.out" $E_OK "Porovnavani null false a true"
testphp "basic/doubleint.php" "empty" "Zapis double a int"
testphp "basic/lokalnostpromenych.php" "empty" "Lokalnost promenych"
testphp "basic/lokalnosttrue.php" "empty" "Lokalnost promenych true"
testphp "basic/returntest.php" empty "Return stringu"
testphp "basic/returntest2.php" empty "Return true hodnoty"
testphp "basic/sortstring.php" empty "test funkce sortstring"
testphp "basic/get_substring.php" empty "test funkce getsubstring"

if [ $passed -eq $cnt ]; then
  msg="Basic result $passed / $cnt"
else
  msg="Basic result ${c_red}$passed ${c_normal} / $cnt"
fi

cnt=0
passed=0
failed=0

echo
echo
#FUNEXP
echo "<?php function aaa(){} return aaa();" > $TMP
$MAIN $TMP >/dev/null 2>/dev/null
rcode=$?
if [ $rcode -eq $E_OK ]; then
  echo "	Implementovano rozsireni FUNEXP:"
  testphp "funexp/fact.php" "empty" "Factorial test"
  testphp "funexp/put_string.php" "empty" "Vyhodnoceni parametru pred volanim funkce put_string"
  testphp "funexp/boolcmp.php" "empty" "Test porovnavani boolovskych hodnot"
  test1 "funexp/strlen.php" empty "basic/zero" $E_OK "Test interni funkce strlen"
  test1 "funexp/chybnyoperator.php" empty empty $E_SYN "chybejici operator mezi funcki a vyrazem"
  if [ $failed -eq 0 ]; then
    BONUS=$[$BONUS+2]
  fi
  failed=0
  echo
fi


#### MINUS
echo "<?php \$a= -  58;" > $TMP
$MAIN $TMP >/dev/null 2>/dev/null
rcode=$?
if [ $rcode -eq $E_OK ]; then
  echo "	Implementovano rozsireni MINUS:"

  test1 "minus/minus.php" "empty" "minus/minus.out" $E_OK "unarni minus"	#pridal edgar
  testphp "minus/minus2.php" "empty" "slozite unarni minus"	#pridal edgar
  test1a $E_TYPE_COMP "<?php \$x= - \"ahoj\";"
  test1a $E_TYPE_COMP "<?php \$x= - true;"

  if [ $failed -eq 0 ]; then
    BONUS=$[$BONUS+2]
  fi
  failed=0
  echo
fi


#### LOGOP
echo "<?php return true or false;" > $TMP
$MAIN $TMP >/dev/null 2>/dev/null
rcode=$?
if [ $rcode -eq $E_OK ]; then
  echo "	Implementovano rozsireni LOGOP:"
  test1a $E_OK "<?php return true || false;"
  test1a $E_OK "<?php return true && false;"
# podle fora je sice mozne provest implicitni pretypovani
# pak by ale jiz nemelo by mozne scitat
  test1a $E_TYPE_COMP "<?php \$x= !0 + 5;"
  test1a $E_TYPE_COMP "<?php \$x= !1 + 5;"
  test1a $E_TYPE_COMP "<?php \$x= (true && 4) + 5;"
  test1a $E_TYPE_COMP "<?php \$x= (true && 4) + 5;"
  test1a $E_TYPE_COMP "<?php \$x= (false || 4) + 5;"
  test1a $E_TYPE_COMP "<?php \$x= -!5; //unarniMINUS"
  
  

  testphp "logop/priority1.php" "empty" "Test priority || a and"
	testphp "logop/logop.php" "empty" "ruzne logicke vyrazy"
	testphp "logop/logop2.php" "empty" "ruzne logicke vyrazy"
	testphp "logop/logop3.php" "empty" "slozity logicky vyraz"
	testphp "logop/neg1.php" "empty" "negace"
	testphp "logop/neg2.php" "empty" "slozitejsi negace"
  if [  $failed -eq 0 ]; then
    BONUS=$[$BONUS+1]
  fi
  failed=0
  echo
fi

#### IFELSE
echo "<?php if(true) {}" > $TMP
$MAIN $TMP >/dev/null 2>/dev/null
rcode=$?
if [ $rcode -eq $E_OK ]; then
  echo "	Implementovano rozsireni IFELSE:"
  test1 elseif/vyraz1.php empty empty $E_OK "Jiny vyraz nez bool v ifu"
  test1 elseif/vyraz2.php empty empty $E_OK "Jiny vyraz nez bool v ifu2"
	testphp "elseif/basic1.php" "empty" "zakladni funkcnost"
	testphp "elseif/basic2.php" "empty" "zakladni funkcnost #2"
  if [  $failed -eq 0 ]; then
    BONUS=$[$BONUS+2]
  fi
  failed=0
  echo
fi

#### EXPAND
echo "<?php \$a=5; \$x=\" ahoj \$a \"" > $TMP
$MAIN $TMP >/dev/null 2>/dev/null
rcode=$?
if [ $rcode -eq $E_OK ]; then
  echo "	Implementovano rozsireni EXPAND:"
  if [  $failed -eq 0 ]; then
    # Jen pokud by to nekdo dopsal
    BONUS=$[$BONUS+2]
  fi
  failed=0
  echo
fi

#### FOR
echo "<?php for ( \$x=0; \$x<10; \$x=\$x+1 ) {}" > $TMP
$MAIN $TMP >/dev/null 2>/dev/null
rcode=$?
if [ $rcode -eq $E_OK ]; then
  echo "	Implementovano rozireni FOR:"

  test1a $E_LEX "<?php  for(5.; true)"
  test1a $E_LEX "<?php  for @ 5.; true)"
  test1a $E_SYN "<?php  for(\$x < 5; true)"
  test1a $E_SYN "<?php  for(\$x=5; true; \$x=5) { continue 2; }"
  testphp "for/nasobilka.php" "empty" "zanoreny for cylus"
  testphp "for/break1.php" "empty" "break a continue"
  testphp "for/undef1.php" "empty" "prazdny prikaz for"
  testphp "for/forinfor.php" empty "zanoreny cyklus 4 urovne"

  #specialni test, break continue mimo blok for. chyba 13 nebo 2
  #prvni zjisti jakou chybu vraci a pak ji pouziva dal pro porovnani
  echo "<?php break; " >  $TMP
  $MAIN $TMP >/dev/null 2>/dev/null
  E_BREAK=$?
  if [ $E_BREAK -eq $E_OK ]; then
    echo "${c_red}FAILED$c_normal  break mimo blok for"
    failed=$[$failse+1]
  else
    test1a $E_BREAK "<?php if(true){ for(\$i=5; 1; \$i=1) {} break;} else {}"
    test1a $E_BREAK "<?php if(true){continue;}else{}"
  fi


  if [  $failed -eq 0 ]; then
    BONUS=$[$BONUS+2]
  fi
  failed=0
  echo
fi

#### DEFARG
echo "<?php function aaa(\$abc = 1) {}" > $TMP
$MAIN $TMP >/dev/null 2>/dev/null
rcode=$?
if [ $rcode -eq $E_OK ]; then
  echo "	Implementovano rozireni DEFARG:"
  test1 "defarg/defarg1.php" empty empty $E_MISPAR "Kdyz, ma parametr def. hodnotu, tak ji musi mit vsechny dalsi"
  test1 "defarg/lex1.php" empty empty $E_LEX "Lexikalni chyba ve vychozim parametru"
  test1 "defarg/redefpar1.php" empty empty $E_OTHER "Redefinice parametru funkce"
  test1a $E_SYN "<?php function aaa ( \$x = \$a){}"
  testphp  "defarg/defarg2.php" empty "Test zakladni funkcnosti rozsireni"
  testphp  "defarg/retezec.php" empty "Retezec jako def argument"

  if [  $failed -eq 0 ]; then
    BONUS=$[$BONUS+1]
  fi
  failed=0
  echo
fi

## ostatni testy kombinujici vice rozsireni
echo "******* COMBO TESTS *******"

test1a $E_SYN "<?php function aaa ( \$x = - 5){} //MINUS DEFARG"
testphp "all/factor.php" "all/factor.in" "Faktorizace cisel. FUNEXP,FOR,IFELSE"
echo
echo $msg

if [ $passed -eq $cnt ]; then
  echo "Bonus result $passed / $cnt"
else
  echo "Bonus result ${c_red}$passed ${c_normal} / $cnt"
fi
 
if [ $(($BONUS%2)) -eq 1 ]; then
  echo "Extra points $((($BONUS)/2)).5"
else
  echo "Extra points $((($BONUS)/2))"
fi

echo "******* nejspis nebude testovano *******"
test1 "basic/lexinphp.php" empty empty $E_LEX "Lexikalni chyba v tokenu <?php na zacatku souboru"
test1 "basic/lexinphp2.php" empty empty $E_LEX "Lexikalni chyba v tokenu <?php uprostred souboru"

rm -f $STDOUT $STDERR $TMPFILE empty $TMP $PHPOUT $PHPERR $VALLOG $LASTFILELOG
