<?php

function myfabs($x)
{
  if(doubleval($x)<0.0){
    return -$x;
  }  else {
    return  $x;
  }
}

function mysqrt($x)
{
  $ival=$x;
  $ival_old=1.0;// rozilna hodnota od x

  if(doubleval($x)<0.0){
    return false;
  }
  if(doubleval($x) === 0.0){
    return 0.0;
  }

  while(myfabs($ival-$ival_old)>0.0)  // pri x=1 se cyklus neprovede, vysledek 1
  {
    $ival_old=$ival;
    $ival=0.5*($ival+$x/$ival);
  }
  return $ival;
}




/**
 * Rozlozi cislo n >= 2 na prvociselne soucinitele.
 * Soucinitele vypise na obrazovku (do konzole).
 *
 * @param $n cislo, ktere chceme faktorizovat, n >= 2
 */
 
function faktorizuj($n) {
    $zbytek = $n;
    for ($i = 2; $i <= intval(mysqrt($zbytek)); $i=$i+1) {
        // Pokud jsme vyzkouseli vsechna cisla mensi nez odmocnina ze zbytku
        //  a zadne z nich nedelilo zbytek, je zbytek prvocislo a muzeme
        //  ho rovnou vypsat.

        // Jinak zkousime delit dalsim potencialním prvoèinitelem, dokud
        //  se to dari.
        while ( ($zbytek - intval($zbytek / $i)*$i) === 0) {
            $zbytek = intval($zbytek / $i);
            $x=put_string(strval($i) . " ");
        }
    }
    // Pokud je zbytek vetsi nez 1, pak je to prvocislo,
    //  ktere je treba vypsat take.
    if ($zbytek > 1) {
        $x=put_string($zbytek);
    }
}

$str = get_string();

while($str !== ""){
  $x=faktorizuj(intval($str));
  $x=put_string("\n");
  $str= get_string();
}
