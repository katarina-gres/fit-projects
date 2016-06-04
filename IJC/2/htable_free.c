
/*
 * htable_free.c
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include "htable.h"

/*
 * Funkce, ktera zrusi celou hashovaci tabulku (nejprve se zavola funkce
 * htable_clear pro zruseni obsahu).
 * @param table Hashovaci tabulka, ktera se ma zrusit.
 */
void htable_free(htable_t *table)
{
   if(table == NULL)
      return;
   htable_clear(table);
   
   free((void *)table);
}
