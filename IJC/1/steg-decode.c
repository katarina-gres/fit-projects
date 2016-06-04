/*
 * steg-decode.c
 * Reseni IJC-DU, priklad b), 5. 3. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#include "bit-array.h"
#include "ppm.h"
#include "eratosthenes.h"
#include "error.h"

#define BYTE CHAR_BIT

/* Mozne stavove kody funkce DecodeHiddenMessage. */
typedef enum
{
   EOK, /* V poradku */
   EMEMORY, /* Chyba pri alokovani pameti. */
   EPRINTF, /* Ve zprave nalezen netisknutelny znak. */
   EEND, /* Zprava byla nekorektne ukoncena. */
   EUNKNOWN /* Neznama chyba. */
} TERR;

/* Chybove zpravy pro jednotlive chyby. */
char * ERRORMSG[] = {
   "V poradku.",
   "Chyba pri alokovani pameti.",
   "Ve zprave byl nalezen netisknutelny znak.",
   "Zprava nebyla korektne ukoncena.",
   "Neznama chyba."
};

int DecodeHiddenMessage(struct ppm *img);

/* Hlavni funkce main. */
int main(int argc, char *argv[])
{
   if(argc != 2)
   {
      FatalError("Chybne parametry prikazove radky.");
   }
   
   struct ppm * img = ppm_read(argv[1]);
   if(img == NULL)
   {
      FatalError("Chyba pri zpracovavani obrazku.");
   }
   
   int ret = DecodeHiddenMessage(img);
   if(ret != EOK)
   {
      free(img);
      if(ret > EUNKNOWN || ret < EOK)
         ret = EUNKNOWN;
      FatalError(ERRORMSG[ret]);
   }

   free(img);
   return 0;
}

/* 
 * Funkce, ktera dekoduje zpravu ukrytou v obrazku PPM.
 * param img Nacteny PPM obrazek
 * return Kod, oznacujici uspesnost vylusteni. 
 */
int DecodeHiddenMessage(struct ppm *img)
{
   unsigned long bits = img->xsize * img->ysize * 3L;
   unsigned long * array = malloc(sizeof(unsigned long) * 
      ComputeArraySize(bits));
   if(array == NULL)
      return EMEMORY;
   array[0] = bits;
   Eratosthenes(array);
   
   char charArray[2] = {1, 0};
   unsigned short bitIndex = 0;
   for(unsigned long i = 2; i < bits; i++)
   {
      if(GetBit(array, i))
         continue;

      DU1_SET_BIT_(charArray, bitIndex, (img->data[i] & 1));
      if(++bitIndex == BYTE)
      {
         if(charArray[1] == '\0')
         {
            free(array);
            return EOK;
         }
         if(!isprint(charArray[1]))
         {
            free(array);
            return EPRINTF;
         }
         putchar(charArray[1]);
         
         charArray[1] = 0;
         bitIndex = 0;
      }
   }
      
   free(array);
   return EEND;
}
