
/*
 * io.c
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include "io.h"

/*
 * Funkce, ktera nacte slovo ze zadaneho streamu.
 * @param s Buffer pro ulozeni slova.
 * @param max Maximalni pocet nactenych znaku.
 * @param f Vstup, ze ktereho se maji cist znaky.
 */
int fgetword(char *s, int max, FILE *f)
{
   if(s == NULL || f == NULL || feof(f))
      return EOF;
   int state = STPREPARE;
   int actualChar = 0;
   int counter = 0;
   int chars = 0;
   
   while((actualChar = fgetc(f)) != EOF)
   {
      switch(state)
      {
         case STPREPARE:
            if(!isspace(actualChar))
            {
               ungetc(actualChar, f);
               state = STREAD;
            }
            break;
            
         case STREAD:
            if(isspace(actualChar) || counter >= max - 1)
            {
               ungetc(actualChar, f);
               state = STIGNORE;
            }
            else
            {
               s[counter++] = actualChar;
               chars++;
            }
            break;
            
         case STIGNORE:
            if(isspace(actualChar))
            {
               s[counter] = '\0';
               return chars;
            }
            chars++;
            break;
      }
   }
   if(counter != 0)
   {
      s[counter] = '\0';
      return chars;
   }
   
   return EOF;
}
