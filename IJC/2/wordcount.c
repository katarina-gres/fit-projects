
/*
 * wordcount.c
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include <stdio.h>
#include <stdbool.h>
#include "htable.h"
#include "io.h"

/* Velikost tabulky by mela byt co nejvzdalenejsi od 2 po sobe 
 * jdoucich hodnot mocnin 2 a melo by to byt prvocislo. */
#define HTABLE_SIZE 12289
/* Maximalni delka slova. */
#define WORD_LENGTH 256

void showItem(const char * const key, unsigned int value);

int main(void)
{
   char word[WORD_LENGTH];
   htable_listitem * item = NULL;
   htable_t * table = htable_init(HTABLE_SIZE);
   if(table == NULL)
   {
      fprintf(stderr, "Chyba pri vytvareni hashovaci tabulky.\n");
      return 1;
   }
   
   int state = 0;
   bool first = true;
   while((state = fgetword(word, WORD_LENGTH, stdin)) != EOF)
   {
      if(first && state > WORD_LENGTH - 1)
      {
         fprintf(stderr, "Byla prekrocena maximalni delka slova. Budu pokracovat se zkracenymi slovy.\n");
         first = false;
      }
      item = htable_lookup(table, word);
      if(item == NULL)
      {
         fprintf(stderr, "Chyba pri pridavani polozky do tabulky.\n");
         break;
      }
      item->data++;
   }
   
   htable_foreach(table, showItem);
   //htable_statistics(table);
   htable_free(table);
   table = NULL;
   
   return 0;
}

/*
 * Funkce, ktera se preda funkci htable_lookup, slouzi k vytisknuti
 * obsahu hashovaci tabulky.
 * @param key Klic polozky.
 * @param value Hodnota polozky.
 */
void showItem(const char * const key, unsigned int value)
{
   if(key == NULL)
      return;
   printf("%s\t%d\n", key, value);
}
