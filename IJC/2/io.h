
/*
 * io.h
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#ifndef IO_H_
#define IO_H_

#include <stdio.h>
#include <ctype.h>

/*
 * Jednotlive stavy pri nacitani slov.
 */
#define STPREPARE 5
#define STREAD 6
#define STIGNORE 7

int fgetword(char *s, int max, FILE *f);

#endif
