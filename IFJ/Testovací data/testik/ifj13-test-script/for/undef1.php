<?php

//teoreticky nekonecna smycka
for($i = 0; ; $i = $i + 1) {
	if($i === 50) {break;}
	else {$x=put_string($i);}
}

//$i by melo zustat z minula 50
for(;$i !== 0; $i = $i - 1) {
	$x=put_string($i);
}

for($j = 42; $j >= $i;){
	$j = $j-1;
	$x=put_string($j);
}
