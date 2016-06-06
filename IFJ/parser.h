// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdbool.h>

#include "ial.h"
#include "adt.h"

/* Navratove konstanty. */
#define SYNTAX_OK 0
#define SYNTAX_ERROR 2
#define LEX_ERROR 1
#define SEM_ERROR 3
#define PARAMS_ERROR 4
#define INTERN_ERROR 99

#define HTABLESIZE 512
#define EOK 42

/* Makro pro nacteni dalsiho tokenu. */
#define GetToken(actToken) actToken = get_token(); \
   if(actToken == NULL) \
      return LEX_ERROR

int createTableFuncList(void);
int insertNewTable(TlistHtable *dest);
bool searchUndefinedFunctions(Thtable *table);
ThtableItem * searchTableList(TlistHtable *table, TToken *token);
int parser(void);
int program(void);
int listStatement(void);
int bodyStatement(void);
int params(void);
int parFirst(void);
int parNext(void);
int assign(TToken **outInstr);
int value(void);
int args(void);
int argFirst(void);
int argNext(void);
int forAssign(int prev);

#endif
