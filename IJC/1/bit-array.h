/*
 * bit-array.h
 * Reseni IJC-DU, priklad a), 5. 3. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 * 
 * Soubor popisuje makra a inline funkce pro práci s bitarray. 
 */

#ifndef BITARRAY_H_
#define BITARRAY_H_

#include <limits.h>
#include "error.h"

typedef unsigned long BitArray_t[];

/* Chybovy kod pri cteni za hranici bitarray. */
#define OVER_ERROR 2

/* Pocet bitu, ktere zabira unsigned long.  */
#define LongBits (sizeof(unsigned long) * CHAR_BIT)

/* Makro, ktere zjisti, zda je nutne k velikosti bitarray pricist 1. */
#define ExtendedSize(size) (((size) % LongBits == 0) ? 0 : 1)

/* Makro, ktere spocita velikost pole typu unsigned long aby se do ni 
 * veslo bitSize bitu. */
#define ComputeArraySize(bitSize) (1 + (bitSize) / LongBits +\
   ExtendedSize(bitSize))

/* Makro, ktere vytvori bitarray a vynuluje ho. Na 1. pozici je ulozena 
 * informace o poctu ulozenych bitu pro kontrolu mezi. */
#define BitArray(array, size) unsigned long array[ComputeArraySize(size)] = {size, 0}

/* Makro, ktere zjisti velikost datoveho typu bitarray v bitech. */
#define TypeSize(t) (sizeof(t[0]) * CHAR_BIT)

/* Makro pro zjisteni jaky bit se nachazi na pozici i v bitarray p. */
#define DU1_GET_BIT_(p,i) ((p[(i) / TypeSize(p) + 1] >> \
   ((i) % TypeSize(p))) & 1)

/* Makro pro nastaveni bitu na indexu i na hodnotu b v bitarray p. */
#define DU1_SET_BIT_(p,i,b)  (p[(i) / TypeSize(p) + 1] = ((b) == 0 ? \
   ((p[(i) / TypeSize(p) + 1]) & ~(1UL << ((i) % TypeSize(p)))):\
   ((p[(i) / TypeSize(p) + 1]) | (1UL << ((i) % TypeSize(p))))))

/* ------------ NDEF USE_INLINE--------------- */
#ifndef USE_INLINE

/* Makro, ktere zjisti velikost bitarray v bitech. */
#define BitArraySize(array) array[0]

/* Makro, ktere vrati bit v promenne array na pozici index. */
#define GetBitFromVar(array, index) ((array >> (index)) & 1)

/* Makro, ktere vrati bit v bitarray array na pozici index. Je 
 * kontrolován přístup za hranici pole. */
#define GetBit(array, index) (((index) < BitArraySize(array)) ? \
   GetBitFromVar(array[(index) / LongBits + 1], (index) % LongBits):\
   (FatalError("Index %ld mimo rozsah 0..%ld", (long)(index), BitArraySize(array) - 1), OVER_ERROR))

/* Makro vrati index v poli kde se vyskytuje bit na indexu index. */
#define GetArrayIndexFromBit(index) ((index) / LongBits + 1)

/* Makra, ktera nastavi odpovidajici bit na hodnotu 0/1 v promenne array. */
#define SetOne(array, index) ((array) | (1UL << (index)))
#define SetZero(array, index) ((array) & ~(1UL << (index)))

/* Makro, ktere nastavi v bitarray bit na pozici index na hodnotu 
 * value % 2. */
#define SetBit(array, index, value) (((index) < BitArraySize(array)) ? \
   (array[GetArrayIndexFromBit(index)] = ((value) == 0 ? \
   SetZero(array[GetArrayIndexFromBit(index)], (index) % LongBits) :\
   SetOne(array[GetArrayIndexFromBit(index)], (index) % LongBits))) :\
   (FatalError("Index %ld mimo rozsah 0..%ld", (long)(index), BitArraySize(array) - 1), OVER_ERROR))

#else /* ------------- DEF USE_INLINE--------------- */

/* Inline funkce, ktera zjisti velikost bitarray v bitech. */
inline unsigned long BitArraySize(BitArray_t array)
{
   return array[0];
}

/* Inline funkce, ktera vrati bit v promenne array na pozici index. */
inline unsigned long GetBitFromVar(unsigned long var, unsigned long index) 
{
   return (var >> (index)) & 1;
}

/* Inline funkce, ktera vrati bit v bitarray array na pozici index. Je 
 * kontrolován přístup za hranici pole. */
inline unsigned long GetBit(BitArray_t array, unsigned long index)
{
   if(index >= BitArraySize(array))
   {
      FatalError("Index %ld mimo rozsah 0..%ld", (long)(index), 
         BitArraySize(array) - 1);
      return OVER_ERROR;
   }
   
   return GetBitFromVar(array[index / LongBits + 1], 
      index % LongBits);
}

/* Inline funkce vrati index v poli kde se vyskytuje bit na indexu index. */
inline unsigned long GetArrayIndexFromBit(unsigned long index)
{
   return index / LongBits + 1;
}

/* Inline funkce, ktera nastavi odpovidajici bit na 
 * hodnotu 0/1 v promenne array. */
inline unsigned long SetOne(unsigned long array, unsigned long index)
{
   return array | (1UL << (index));
}
inline unsigned long SetZero(unsigned long array, unsigned long index)
{
    return array & ~(1UL << (index));
}

/* Inline funkce, ktera nastavi v bitarray bit na pozici index na hodnotu 
 * value % 2. */
inline void SetBit(BitArray_t array, unsigned long index, 
   unsigned short value)
{
   if(index >= BitArraySize(array))
   {
      FatalError("Index %ld mimo rozsah 0..%ld", (long)(index), 
         BitArraySize(array) - 1);
   }
      
   if(value == 0)
   {
      array[GetArrayIndexFromBit(index)] = SetZero(array
         [GetArrayIndexFromBit(index)], index % LongBits);
   }
   else
   {
      array[GetArrayIndexFromBit(index)] = SetOne(array
         [GetArrayIndexFromBit(index)], index % LongBits);
   }
}

#endif

#endif
