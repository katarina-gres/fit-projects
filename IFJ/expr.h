// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

/*
 * Hlavickovy souboru pro implementaci syntaktické analýzy ze zdola nahoru.
 */

#ifndef _EXPR_H_
#define _EXPR_H_

#include "keywords.h"
#include "lex.h"

#define TABLESIZE 15

#define ENOTERM 200
#define DOLLAR 4242

#define STERROR 0
#define STOK 1

#define SATable(x,y) PrecedenceTable[(x)][(y)]

/* Makra pro rozliseni terminalu. */
#define IsConstant(c) ((c) >= 20 && (c) <= 24)
#define IsOperator(c) (((c) >= 11 && (c) <= 18) || (c) == _LESS || (c) == _GREATER || (c) == _DOT)
#define IsEndSymbol(c) ((c) == _SEMICOLON || (c) == _LBRACE)

/* Struktura pro obsah zasobniku. */
typedef struct STACKDATA
{
   int data;
   TToken* token;
   struct STACKDATA *next;
} STACKDATA;

/* Samotny zasobnik. */
typedef struct
{
   STACKDATA *top;
} STACK;

void stackInit(STACK *stack);
STACKDATA *stackPop(STACK *stack);
int stackPush(STACK *stack, int value, TToken *token);
int stackEmpty(STACK *stack);
STACKDATA* topStackTerminal(STACK *stack);
int evaluateExpression(TToken *actToken, TToken **lastToken, 
   TToken **outInstr);
int replaceRule(STACK *stack, TToken **outInstr);
int pushToken(STACK *stack, TToken * token);
void printStack(STACK *stack);
void stackClear(STACK *stack);
void freeDataStructures(STACK *stack);


#endif
