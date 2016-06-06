<?php

$t = true;
$f = false;
if($t and $t) {
	$t = false;
	$x=put_string("jo");
}
else {
	$x=put_string("ne");
}

if($t and $t) {
	$t = true;
	$x=put_string("no");
}
else {
	$x=put_string("jo");	// tohle je dobre
}

/*
 * komentar
 * */

if(true && (!true or 0) and (!true && true or 1 or !false) and !("string" || true and null) and $t && "" and (!true or false || false) and 1 && !(false and null and 48 || 0.15)) {
	$x=put_string("ne");
}
else {
	$x=put_string("jo");
}
