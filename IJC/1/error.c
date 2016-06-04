/*
 * error.c
 * Reseni IJC-DU, priklad b), 5. 3. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "error.h"

/*
 * Funkce, ktera vypise zformatovane parametry na standardni 
 * chybovy vystup.
 */
void Error(const char *fmt, ...)
{
   va_list arguments;
   
   fprintf(stderr, "CHYBA: ");
   va_start(arguments, fmt);
   vfprintf(stderr, fmt, arguments);
   va_end(arguments);
}

/*
 * Funkce, ktera vypise zformatovane parametry na standardni 
 * chybovy vystup a ukonci program
 */
void FatalError(const char *fmt, ...)
{
   va_list arguments;
   
   fprintf(stderr, "CHYBA: ");
   va_start(arguments, fmt);
   vfprintf(stderr, fmt, arguments);
   va_end(arguments);
   
   exit(1);
}
