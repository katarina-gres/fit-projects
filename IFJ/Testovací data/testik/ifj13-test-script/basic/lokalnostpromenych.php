<?php

$prom = 2;
$x=put_string($prom,"\n");
function x() {
	$prom = 5;
	return $prom;
}

$x=put_string($prom,"\n");
$x=put_string(x(),"\n");
$x=put_string($prom,"\n");
$prom=x();
$x=put_string($prom,"\n");
