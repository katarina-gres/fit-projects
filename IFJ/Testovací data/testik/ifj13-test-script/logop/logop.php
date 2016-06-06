<?php

$c = true;
$d = !(!!$c);
if($c and $d) {
	$x=put_string("oba");
}
elseif($c or $d) {
	if($c) {
		$x=put_string("jen c");
	}
	elseif($d) {
		$x=put_string("jen d");
	}
	else {
		$x=put_string("chyba");
	}
}
$e = !false;
if($e and $c and $d or $e and true) {
	$x=put_string("\n","hura");
}


$x=put_string("\n");
