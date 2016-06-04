
/*
 * hash_function.c
 * Reseni IJC-DU, priklad b), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 * 
 * Rozhrani k hashovaci tabulce.
 */

#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include <stdlib.h>
#include <stdio.h>

/*
 * Polozka v hashovaci tabulce.
 */
typedef struct htable_listitem
{
   char * key;
   unsigned int data;
   struct htable_listitem * next;
} htable_listitem;

/*
 * Hashovaci tabulka.
 */
typedef struct
{
   unsigned int htable_size;
   htable_listitem * ptr[];
} htable_t;

/* Funkcni prototyp volany na kazdy prvek ve funkci htable_foreach. */
typedef void (*foreach_func)(const char * const, unsigned int);

/* Prototypy funkci. */
htable_t * htable_init(unsigned int size);
void htable_foreach(const htable_t * table, foreach_func func);
htable_listitem * htable_lookup(htable_t * table, const char *key);
void htable_remove(htable_t * table, const char * key);
void htable_clear(htable_t * table);
void htable_free(htable_t * table);
void htable_statistics(htable_t * table);

#endif
