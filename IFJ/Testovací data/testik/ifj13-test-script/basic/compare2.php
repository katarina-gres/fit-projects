<?php

if( false === false ) { 
  $x=put_string("true\n");
} else {
  $x=put_string("CHYBA: ");
  $x=put_string("false\n");
}

if( false !== false) { 
  $x=put_string("CHYBA: ");
  $x=put_string("true\n");
} else {
  $x=put_string("false\n");
}

if( true === true ) { 
  $x=put_string("true\n");
} else {
  $x=put_string("CHYBA: ");
  $x=put_string("false\n");
}

if( false === true ) { 
  $x=put_string("CHYBA: ");
  $x=put_string("true\n");
} else {
  $x=put_string("false\n");
}

if( null === null ) { 
  $x=put_string("true\n");
} else {
  $x=put_string("CHYBA: ");
  $x=put_string("false\n");
}



if( 5.0 !== 5 ) { 
  $x=put_string("true\n");
} else {
  $x=put_string("CHYBA: ");
  $x=put_string("false\n");
}

if( false < true ) { 
  $x=put_string("true\n");
} else {
  $x=put_string("CHYBA: ");
  $x=put_string("false\n");
}

if( false > true ) { 
  $x=put_string("CHYBA: ");
  $x=put_string("true\n");
} else {
  $x=put_string("false\n");
}

if( false > false ) { 
  $x=put_string("CHYBA: ");
  $x=put_string("true\n");
} else {
  $x=put_string("false\n");
}

if( false >= false ) { 
  $x=put_string("true\n");
} else {
  $x=put_string("CHYBA: ");
  $x=put_string("false\n");
}

if( 5 > 6 === 8 > 8) { 
  $x=put_string("true\n");
} else {
  $x=put_string("CHYBA: ");
  $x=put_string("false\n");
}

