/*
 * steg-decode.c
 * Reseni IJC-DU, priklad a), 5. 3. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 * 
 * Doba behu programu:
 *  -- se zapnutym prepinacem -O2: makra (1.4s) inline (1.34s)
 */

#include <stdio.h>

#include "bit-array.h"
#include "error.h"
#include "eratosthenes.h"

/* Horni mez pro hledani prvocisel. */
#define N 89000000UL
/* Kolik poslednich prvocisel se ma vypsat. */
#define LAST_N 10

void PrintPrimes(BitArray_t array);

int main(void)
{
   BitArray(array, N);
   Eratosthenes(array);
   
   PrintPrimes(array);
   
   return 0;
}

/*
 * Funkce, ktera vytiskne poslednich LAST_N prvocisel zadanych 
 * polem array.
 * param array Bitarray s prvocisly. 
 */
void PrintPrimes(BitArray_t array)
{
   unsigned short primesCount = 0;
   unsigned long i;
   
   for(i = BitArraySize(array) - 1; i >= 2 && primesCount < LAST_N; i--)
   {
      if(!GetBit(array, i))
         primesCount++;
   }
   
   for(; i < BitArraySize(array); i++)
   {
      if(GetBit(array, i) == 0)
      {
         printf("%lu\n", i);
      }
   }
}
