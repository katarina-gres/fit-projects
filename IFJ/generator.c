// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

/*
 * Pomocne funkce pro generovani vnitrniho (triadresneho) kodu.  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generator.h"
#include "parser.h"
#include "interpret.h"
#include "global.h"

/*
 * Funkce, ktera vytvori nazev docasne promenne (bez $, aby nedochazelo
 * ke kolizim).
 * @return Unikatni nazev docasne promenne.
 */
char *createTmpVarName(void)
{
   static int counterVar = 0;
   char *name = createTmpName("tmp", counterVar);
   counterVar++;
   
   return name;
}

/*
 * Vnitrni funkce pro spojeni retezce a cisla.
 * @param name Retezec
 * @param counter Cislo pro spojeni
 * @return Spojeny retezec.
 */
char *createTmpName(const char *name, int counter)
{
   char *tmpName = malloc(sizeof(char) * 10);
   if(tmpName == NULL)
      return NULL;
   
   sprintf(tmpName, "%s%d", name, counter);
   
   return tmpName;
}

/*
 * Funkce, ktera vytvori unikatni nazev labelu.
 * @return Unikatni nazev labelu.
 */
char *createLabelName(void)
{
   static int counterLabel = 0;
   char *name = createTmpName("label", counterLabel);
   counterLabel++;
   
   return name;
}

/*
 * Funkce, ktera vytvori label s unikatnim jmenem ulozenym v tokenu.
 * @return Vytvoreny label, NULL v pripade chyby.
 */
TToken *createLabel(void)
{
   TToken *label = malloc(sizeof(TToken));
   if(label == NULL)
      return NULL;
   if(token_list_insert(g_token_list, label) == NULL)
   {
      free((void *)label);
      return NULL;
   }
   
   label->type =  _STRINGVALUE; //_STRINGVALUE; /* Kvuli uvolnovani */
   label->name = NULL;
   label->value.stringVal = createLabelName();
   
   return label;
}

/*
 * Funkce, ktera prevadi konstanty z KEYWORDS na cisla instrukci
 * interpretu.
 * @param keyword Jaka konstanta se ma prevest.
 * @return Prevedena konstanta.
 */
int convertKeywordToInst(int keyword)
{
   if(keyword == _DOT)
      return I_KONKATENACE;
   return keyword;
}

/*
 * Funkce, ktera vytvori instrukci aritmeticke, logicke nebo relacni operace.
 * @param operand1 1. operand
 * @param operand2 2. operand
 * @param operator Operator
 * @return Vytvorena instrukce.
 */
Tinstr* createArithmeticInstruction(TToken * operand1, 
   TToken * operand2, TToken * operator)
{
   Tinstr *inst = malloc(sizeof(Tinstr));
   if(inst == NULL)
      return NULL;
   TToken *dest = malloc(sizeof(TToken));
   if(dest == NULL)
      return NULL;
   if(token_list_insert(g_token_list,dest) == NULL) return NULL;
   
   dest->type = _VARIABLE;
   dest->name = createTmpVarName();
   
   inst->instr_type = convertKeywordToInst(operator->type);
   inst->adr1 = (void *)operand1;
   inst->adr2 = (void *)operand2;
   inst->adr3 = (void *)dest;
   
   return inst;
}

/*
 * Funkce, ktera vytiskne vygenerovany seznam instrukci.
 * @param list Seznam, ktery se ma vytisknout.
 */
void printInstructionList(TlistInstr *list)
{
   TinstrItem *act = list->first;
   while(act != NULL)
   {
      printInstruction(act->instruction);
      act = act->rptr;
   }
}

/*
 * Funkce pro kontrolni tisk instrukce.
 * @param inst Instrukce, ktera se ma vytisknout.
 */
void printInstruction(Tinstr *inst)
{
   printf("%d: ", inst->instr_type);
   
   printInstrPar((TToken *)inst->adr1);
   printInstrPar((TToken *)inst->adr2);
   printInstrPar((TToken *)inst->adr3);
   putchar('\n');
}

/*
 * Funkce, ktera vytiskne token, ktery reprezentuje cast instrukce.
 * @param in Token, ktery se ma vytisknout.
 */
void printInstrPar(TToken *in)
{
   if(in == NULL)
      return;
   if(in->name == NULL)
   {
      printf("%d ", in->type);
      //if(in->type == _STRINGVALUE)
      //   printf("%s ", in->value.stringVal);
   }
   else
      printf("%s ", in->name);
}

/*
 * Funkce, ktera prida instrukci do seznamu instrukci.
 * @param list Seznam instrukci.
 * @param instrType Typ instrukce.
 * @param o1 - o3 Parametry (operandy) instrukce.
 * @return Navratova hodnota (EOK)
 */
int addInstruction(TlistInstr *list, int instrType, void *o1, 
   void *o2, void *o3)
{
   Tinstr *inst = malloc(sizeof(Tinstr));
   if(inst == NULL)
      return INTERN_ERROR;
   
   inst->instr_type = instrType;
   inst->adr1 = o1;
   inst->adr2 = o2;
   inst->adr3 = o3;
   
   instr_list_insert_last(inst, list);
   
   return EOK;
}

/*
 * Funkce, ktera prida instrukci pro volani funkce (rozliseni, zda 
 * se jedna o vestavenou funkci).
 * @param list Seznam instrukci.
 * @param token Token reprezentujici nazev funkce.
 */
int callFunctionInstruction(TlistInstr *list, TToken *token)
{
   if(strcmp(token->name, "boolval") == 0)
   {
      addInstruction(list, I_BOOLVAL, NULL, NULL, NULL);
   }
   else if(strcmp(token->name, "doubleval") == 0)
   {
      addInstruction(list, I_DOUBLEVAL, NULL, NULL, NULL);
   }
   else if(strcmp(token->name, "intval") == 0)
   {
      addInstruction(list, I_INTVAL, NULL, NULL, NULL);
   }
   else if(strcmp(token->name, "strval") == 0)
   {
      addInstruction(list, I_STRVAL, NULL, NULL, NULL);
   }
   else if(strcmp(token->name, "get_string") == 0)
   {
      addInstruction(list, I_GET_STRING, NULL, NULL, NULL);
   }
   else if(strcmp(token->name, "put_string") == 0)
   {
      addInstruction(list, I_PUT_STRING, NULL, NULL, NULL);
   }
   else if(strcmp(token->name, "strlen") == 0)
   {
      addInstruction(list, I_STRLEN, NULL, NULL, NULL);
   }
   else if(strcmp(token->name, "get_substring") == 0)
   {
      addInstruction(list, I_GET_SUBSTRING, NULL, NULL, NULL);
   }
   else if(strcmp(token->name, "find_string") == 0)
   {
      addInstruction(list, I_FIND_STRING, NULL, NULL, NULL);
   }
   else if(strcmp(token->name, "sort_string") == 0)
   {
      addInstruction(list, I_SORT_STRING, NULL, NULL, NULL);
   }
   else
   {
      TToken *copy = malloc(sizeof(TToken));
      if(copy == NULL)  return INTERN_ERROR;
      if(token_list_insert(g_token_list, copy) == NULL)
      {
         free((void *)copy);
         return INTERN_ERROR;
      }
      copy->type = -1;
      copy->name = NULL;
      copy->value.stringVal = token->name;
      
      addInstruction(list, I_JUMPFCE, NULL, NULL, copy);
   }
   return SYNTAX_OK;
}

/*
 * Funkce pro pridani docane promenne do seznamu instrukci.
 * @param outInstr Obsah docasne promenne.
 * @return Uspech operace.
 */
int AddTemporaryVariable(TToken *outInstr)
{
   TToken *tmpVar = malloc(sizeof(TToken));
   if(tmpVar == NULL)
      return INTERN_ERROR;
   if(token_list_insert(g_token_list, tmpVar) == NULL)
   {
      free((void *)tmpVar);
      return INTERN_ERROR;
   }
      
   tmpVar->name = createTmpVarName();
   if(tmpVar->name == NULL)   return INTERN_ERROR;
   tmpVar->type = _VARIABLE;
         
   addInstruction(g_instr, I_ASSIGN, outInstr, NULL, 
      tmpVar);
      
   return SYNTAX_OK;
}

