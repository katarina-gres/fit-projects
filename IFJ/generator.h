// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

/*
 * Pomocne funkce pro generovani vnitrniho (triadresneho) kodu.  
 */

#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "ial.h"
#include "adt.h"
#include "keywords.h"

char *createTmpVarName(void);
int convertKeywordToInst(int keyword);
Tinstr* createArithmeticInstruction(TToken * operand1, 
   TToken * operand2, TToken * operator);
void printInstructionList(TlistInstr *list);
void printInstruction(Tinstr *inst);
void printInstrPar(TToken *in);
TToken *createLabel(void);
char *createLabelName(void);
char *createTmpName(const char *name, int counter);
int addInstruction(TlistInstr *list, int instrType, void *o1, 
   void *o2, void *o3);
int callFunctionInstruction(TlistInstr *list, TToken *token);
int AddTemporaryVariable(TToken *outInstr);

#endif
