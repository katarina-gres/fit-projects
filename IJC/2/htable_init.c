
/*
 * htable_init.c
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include "htable.h"

/*
 * Funkce, ktera inicializuje hashovaci tabulku.
 * @param size Velikost hashovaci tabulky.
 * @return Vytvorena hashovaci tabulka, NULL pri chybe.
 */
htable_t * htable_init(unsigned int size)
{
   if(size == 0)
      return NULL;
   htable_t * ret = malloc(sizeof(htable_t) + 
      size * sizeof(htable_listitem *));
   if(ret == NULL)
      return NULL;
   
   ret->htable_size = size;
   for(unsigned int i = 0; i < size; i++)
   {
      ret->ptr[i] = NULL;
   }
   
   return ret;
}
