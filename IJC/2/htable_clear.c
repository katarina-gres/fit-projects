
/*
 * htable_clear.c
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include "htable.h"

/*
 * Funkce, ktera vymaze obsah hashovaci tabulky.
 * @param table Hashovaci tabulka, jejiz obsah se ma vymazat.
 */
void htable_clear(htable_t * table)
{
   if(table == NULL)
      return;
   htable_listitem * actualItem = NULL;
   htable_listitem * removal = NULL;
   for(unsigned int i = 0; i < table->htable_size; i++)
   {
      actualItem = table->ptr[i];
      while(actualItem != NULL)
      {
         removal = actualItem;
         actualItem = actualItem->next;
         
         free((void *)removal->key);
         free((void *)removal);
      }
      table->ptr[i] = NULL;
   }
}
