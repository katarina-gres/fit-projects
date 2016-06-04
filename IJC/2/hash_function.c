
/*
 * hash_function.c
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include "hash_function.h"

/*
 * Hashovaci funkce, ktera spocita index na jakem je klic ulozen.
 * @param str Klic.
 * @param htable_size Velikost hashovaci tabulky.
 * @return Index v hashovaci tabulce.
 */
unsigned int hash_function(const char *str, unsigned htable_size) 
{ 
   unsigned int h = 0; 
   unsigned char * p; 
   for(p=(unsigned char*)str; *p!='\0'; p++) 
      h = 31*h + *p; 
   
   return h % htable_size; 
}
