// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#ifndef BUILT_IN
#define BUILT_IN

#define _LENGTH 30
#define BUFFER 16

#include <stdbool.h>
#include "ial.h"
#include "keywords.h"

bool bool_val(TToken *data);
char *str_val(TToken *data, int *errno);
double double_val(TToken *data, int *errno);
int int_val(TToken *data);
char *get_string(void);
int str_len(TToken *data, int *errno);
char *get_substring(char *str, int index_from, int index_to, int *errno);
char *konkatenace(char *str1, char *str2);
#endif
