// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

/*
 * Implementace pomocnych funkci pro semanticky analyzator.
 */

#include "semantic.h"
#include "parser.h"

/*
 * Funkce pro deklaraci funkce (funkce se prida do tabulky symbolu).
 * @param table Tabulka symbolu.
 * @param funcCopy Token reprezentujici jmeno funkce.
 * @param paramsCount Pocet parametru funkce
 * @return Podarilo se vlozit do ts, v pripade redefinice ... SEM_ERROR.
 */
int semFuncDeclaration(Thtable *table, TToken *funcCopy, int paramsCount)
{
   ThtableItem *it = htable_search(table, funcCopy);
   if(it != NULL && it->token->isDefine)
      return SEM_ERROR;
   /* Vlozime funkci do tabulky symbolu. */
   if(it == NULL)
   {
      funcCopy->value.intVal = paramsCount;
      funcCopy->isDefine = true;
      htable_insert(table, funcCopy);
   }
   else
   {
      it->token->isDefine = true;
      if(it->token->value.intVal < paramsCount)
         return PARAMS_ERROR;
      it->token->value.intVal = paramsCount;
   }
   
   return EOK;
}

/*
 * Funkce volana pri volani funkce. Zahrnuje docasne vlozeni 
 * funkce do TS a prepsani poctu parametru.
 * @param table Tabulka symbolu.
 * @param funcCopy Token reprezentujici nazev funkce.
 * @param paramsCount Pocet parametru.
 * @return EOK.
 */
int semFuncCall(Thtable *table, TToken *funcCopy, int paramsCount)
{
   ThtableItem *it = htable_search(table, funcCopy);
   if(it == NULL)
   {
      funcCopy->value.intVal = paramsCount;
      funcCopy->isDefine = false;
      htable_insert(table, funcCopy);
   }
   else
   {
      if(it->token->isDefine)
      {
         if(it->token->value.intVal > paramsCount)
            return PARAMS_ERROR;
      }
      else
      {
         it->token->value.intVal = Min(it->token->value.intVal, 
            paramsCount);
      }
   }
   
   return EOK;
}
