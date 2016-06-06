<?php
$a = 6;
function factorial($n)
{
	if ($n < 2)
	{
		$result = 1;
	}
	else
	{
		$decremented_n = $n - 1;
		$temp_result = factorial($decremented_n);
		$result = $n * $temp_result;
	}
	return $result;
}

$vysl = factorial($a);
$msg = put_string($vysl);
