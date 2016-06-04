
/*
 * htable_remove.c
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include <string.h>
#include "htable.h"
#include "hash_function.h"

/*
 * Funkce, ktera vymaze prvek s klicem key z hasovaci tabulky.
 * @param table Hashovaci tabulka, ze ktere se ma prvek vymazat.
 * @param key Klic k vymazani polozky.
 */
void htable_remove(htable_t * table, const char * key)
{
   if(table == NULL || key == NULL || table->htable_size == 0)
      return;
   unsigned int index = hash_function(key, table->htable_size);
   
   htable_listitem * actual = table->ptr[index];
   htable_listitem * prev = NULL;
   while(actual != NULL)
   {
      if(strcmp(actual->key, key) == 0)
      {
         if(actual->data == 1)
         {
            if(prev == NULL)
               table->ptr[index] = actual->next;
            else
               prev->next = actual->next;
            free((void *)actual->key);
            free((void *)actual);
         }
         else
         {
            actual->data--;
         }
         return;
      }
      prev = actual;
      actual = actual->next;
   }
}
