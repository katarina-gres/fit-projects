#!/bin/bash



TESTS=1
while true; do
  
  if ! ./test.sh ; then
    echo "Prave dobehl ${TESTS}. test"
    break
  fi
 
  
  TESTS=$[$TESTS+1]
done


