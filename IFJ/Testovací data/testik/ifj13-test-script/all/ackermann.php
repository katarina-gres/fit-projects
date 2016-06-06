<?php

function ack($m, $n){
  if ($m === 0) {
    return $n+1;
  } elseif ($m > 0 and $n === 0) {
    return ack($m-1, 1);
  } else {
    return ack($m-1, ack($m, $n-1));
  }
}

$x=put_string(ack(3,8),"\n");
