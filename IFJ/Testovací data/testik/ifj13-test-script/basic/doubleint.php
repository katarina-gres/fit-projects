<?php

/*
 * README:
 * pokud jsem neco pochopil spatne tak mi napiste
 * jinak by to melo ale hazet stejny vysledek jako php
 *  - coz ovsem nedela!?
 * php to bere cislo zacinajici 0 jako osmickove
 * */

//int
$int = 1840150;

//double
$double = 00001.010100101100100110;
$double1 = 15e7;
$double2 = 104E01;
$double3 = 18.015e-2;
$double4 = 0.125E+4;

$a = $int - $double - $double1 + $double2 * $double3 - $double4;
$a = intval($a);
$x=put_string($a);
