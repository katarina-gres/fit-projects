/*
 * steg-decode.c
 * Reseni IJC-DU, priklad b), 5. 3. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include <math.h>
#include "eratosthenes.h"

/*
 * Funkce, ktera implementuje Erastothenovo sito -- algoritmus, ktery 
 * hleda prvocisla mensi nez zadana mez.
 * param array Bitarray, kam se maji prvocisla zapsat.
 */
void Eratosthenes(BitArray_t array)
{
   ClearBitArray(array);
   unsigned long rootSqrt = (unsigned long)sqrt((double)BitArraySize(array));
   SetBit(array, 1, 1); /* Cislo 1 neni prvocislo */
   
   for(unsigned long i = 2; i <= rootSqrt; i++)
   {
      if(GetBit(array, i))
         continue;
      
      for(unsigned long tmp = 2*i; tmp < array[0]; tmp += i)
      {
         SetBit(array, tmp, 1);
      }
   }
}

/*
 * Funkce, ktera vynuluje Bitarray.
 * param array Pole, ktere se ma vynulovat.
 */
void ClearBitArray(BitArray_t array)
{
   unsigned long size = ComputeArraySize(BitArraySize(array));
   for(unsigned long i = 1; i < size; i++)
      array[i] = 0;
}
