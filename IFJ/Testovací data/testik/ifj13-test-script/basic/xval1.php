<?php

$x=boolval("    251");
if($x){ 
  $x=strval("12555.5");
  $x=intval($x);
  $x=$x/5.0;
  $x=strval($x);
  $x=$x . "e-2";
  $x=doubleval($x);
  $x=intval($x);
  $x=put_string($x);
}
else {

}
