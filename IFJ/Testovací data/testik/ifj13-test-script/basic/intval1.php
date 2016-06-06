<?php

$x="   -31";
$_x=intval($x); //vyhodnoti na 0
if($_x) {
  $x=put_string("CHYBA1");
} else {}

$x="   +31";
$_x=intval($x); //vyhodnoti na 0
if($_x) {
  $x=put_string("CHYBA2");
} else {}

$x="   0+31";
$_x=intval($x); //vyhodnoti na 0
if($_x) {
  $x=put_string("CHYBA3");
} else {}
