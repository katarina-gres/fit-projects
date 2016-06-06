<?php


function aaa($a,$b) {
  $x=put_string(intval($a<$b));
  $x=put_string(intval($a<=$b));
  $x=put_string(intval($a>$b));
  $x=put_string(intval($a>=$b));
  $x=put_string(intval($a===$b));
  $x=put_string(intval($a===$a));
  $x=put_string(intval($b===$b));
  $x=put_string(intval($a!==$b));
  $x=put_string(intval($b!==$b));
  $x=put_string(intval($a!==$a));
}


$x=aaa(true,false);
$x=aaa(false,true);
$x=aaa(true,true);
$x=aaa(false,false);
$x=aaa(4>5,5<4);
