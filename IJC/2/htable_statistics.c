
/*
 * htable_statistics.c
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include "htable.h"

/*
 * Funkce, ktera zjisti statistiku o hashovaci tabulke. Minimalni,
 * maximalni a prumernou delku seznamu.
 * @param table Hashovaci tabulka, jejiz statistika se ma zjistit.
 */
void htable_statistics(htable_t * table)
{
   if(table == NULL)
      return;
   if(table->htable_size == 0)
      return;
   unsigned int min = 0, max = 0, counter = 0, sum = 0;
   htable_listitem * item = NULL;
   
   for(unsigned int i = 0; i < table->htable_size; i++)
   {
      item = table->ptr[i];
      counter = 0;
      while(item != NULL)
      {
         counter++;
         item = item->next;
      }
      sum += counter;
      if(counter < min || i == 0)
         min = counter;
      if(counter > max || i == 0)
         max = counter;
   }
   
   printf("Min: %d, max: %d, prumer: %.2f", min, max, 
      (double)sum / table->htable_size);
}
