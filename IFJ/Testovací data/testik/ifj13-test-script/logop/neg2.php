<?php

$x = false;
if(!$x) {
	$a=put_string("o");
}
else {
	$a=put_string("chyba\n");
}

if($x or (!true and true) || !!false) {
	$x=put_string("chyba\n");
}

if(!!true){
	$a=put_string("o");}

if(!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!false and !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!true) {
	$x=put_string("k");
}
