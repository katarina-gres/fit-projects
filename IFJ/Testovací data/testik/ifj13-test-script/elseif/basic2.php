<?php

function test() {
	$x=put_string("o");
	return $x;
}

if(null) {
	$_=put_string("chyba\n");
}
if(1+1*2-3) {
	$_=put_string("chyba\n");
}elseif(""){}elseif(false){}else{
	$_=put_string("o");}

if(test() > 2 * 5 - 1 === false){
	$_=put_string("k");}

//komentar

/*
 * viceradkovy komentar */

else{
	$_=put_string("chyba\n");}
