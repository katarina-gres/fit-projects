<?php

for($i = 0; $i < 7; $i = $i + 1) {
	if($i === 5) {break;}
	else {$x=put_string($i);}
}

for($i = 0; $i < 7; $i = $i + 1) {
	if($i > 5) {continue;}
	else {$x=put_string($i);}
}
