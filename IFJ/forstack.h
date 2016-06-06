// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#ifndef _FOR_STACK_H_
#define _FOR_STACK_H_

#include <stdlib.h>
#include "ial.h"

typedef struct STACKFORDATA
{
   TToken* labelBreak;
   TToken* labelContinue;
   struct STACKFORDATA *next;
} STACKFORDATA;

typedef struct
{
   STACKFORDATA *top;
} STACKFOR;

void stackForInit(STACKFOR *stack);
STACKFORDATA *stackForPop(STACKFOR *stack);
int stackForPush(STACKFOR *stack, TToken *labelBreak, TToken *labelContinue);
int stackForEmpty(STACKFOR *stack);
void stackForClear(STACKFOR *stack);
STACKFORDATA * stackForTop(STACKFOR *stack);

#endif
