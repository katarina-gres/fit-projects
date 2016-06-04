
/*
 * htable_lookup.c
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include <string.h>
#include "htable.h"
#include "hash_function.h"

/*
 * Funkce, ktera vyhleda polozku se zadanym klicem. Pokud neni polozka
 * nalezena, vytvori se novy zaznam v hashovaci tabulce.
 * @param table Hashovaci tabulka do ktere se ma pridat polozka.
 * @param key Klic.
 * @return Polozka s klicem key, v pripade chyby NULL.
 */
htable_listitem * htable_lookup(htable_t * table, const char *key)
{
   if(table == NULL || key == NULL || table->htable_size == 0)
      return NULL;
   unsigned int index = hash_function(key, table->htable_size);
   
   htable_listitem * actual = table->ptr[index];
   while(actual != NULL)
   {
      if(strcmp(actual->key, key) == 0)
      {
         return actual;
      }
      actual = actual->next;
   }
   
   /* Vlozeni polozky */
   htable_listitem * newItem = malloc(sizeof(htable_listitem));
   if(newItem == NULL)
      return NULL;

   newItem->key = malloc(strlen(key) + 1);
   if(newItem->key == NULL)
      return NULL;
      
   newItem->data = 0;
   strcpy(newItem->key, key);
   newItem->next = table->ptr[index];
   table->ptr[index] = newItem;
   
   return newItem;
}
