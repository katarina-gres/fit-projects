// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

/*
 * Implementace pomocnych funkci pro semanticky analyzator.
 */

#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "ial.h"

#define Min(a,b) (((a) > (b)) ? (b) : (a))

int semFuncDeclaration(Thtable *table, TToken *funcCopy, int paramsCount);
int semFuncCall(Thtable *table, TToken *funcCopy, int paramsCount);

#endif
