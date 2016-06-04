
/*
 * htable_foreach.c
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include "htable.h"

/*
 * Funkce, ktera projde vsechny prvky v tabulce a na kazdou zavola
 * funkci func.
 * @param table Tabulka, jejiz prvky se maji projit.
 * @param func Funkce, ktera se ma zavolat na kazdy prvek.
 */
void htable_foreach(const htable_t * table, foreach_func func)
{
   if(table == NULL || func == NULL)
      return;
   htable_listitem * actual;
   for(unsigned int i = 0; i < table->htable_size; i++)
   {
      actual = table->ptr[i];
      while(actual != NULL)
      {
         func(actual->key, actual->data);
         actual = actual->next;
      }
   }
}
