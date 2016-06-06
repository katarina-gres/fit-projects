// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

/*
 * Syntakticka analyza zhora dolu.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lex.h"
#include "keywords.h"
#include "expr.h"
#include "generator.h"
#include "semantic.h"
#include "interpret.h"
#include "global.h"
#include "instruction.h"
#include "forstack.h"

//#define PARSER_DEBUG
#define DPRINT
#undef DPRINT

/* Aktualni token. */
TToken *actToken;
/* Pocet parametru funkce. */
int paramsCount = 0;

/* Zasobnik pro uchovavani labelu BREAK a CONTINUE cyklu for.
 * Pouzito pro vnorene cykly for. */
STACKFOR *stackFor = NULL;

#define ActualListTable (g_htable)
#define ActualTable (g_htable->act->table)

/*
 * Vytvoreni seznam TS pro funkci.
 * @return INTERN_ERROR v pripade chyby pameti, jinak EOK
 */
int createTableFuncList(void)
{
   /*funcList = htable_list_init();
   if(funcList == NULL)
      return INTERN_ERROR;
   insertNewTable(funcList);*/
   
   return EOK;
}

/*
 * Funkce pro vytvoreni nove TS a jeji vlozeni do seznamu.
 * @param dest Seznam, kam se ma tabulka ulozit.
 * @return INTERN_ERROR v pripade chyby pameti, jinak EOK.
 */
int insertNewTable(TlistHtable *dest)
{
   Thtable * table = htable_init(HTABLESIZE);
   if(table == NULL)
      return INTERN_ERROR;
   htable_list_insert(dest, table);
   
   return EOK;
}


/*
 * Funkce, ktera vyhleda funkce, ktere jsou zapsany v tabulce symbolu 
 * (byly zavolany), ale nebyly deklarovany -- semanticka chyba.
 * @param table Tabulka symbolu pro funkce.
 * @return false Nalezena nedeklarovana funkce, jinak true
 */
bool searchUndefinedFunctions(Thtable *table)
{
   ThtableItem *act = NULL;
   for(int i = 0; i < table->size; i++)
   {
      act = table->item[i];
      while(act != NULL)
      {
         if(act->token->type == _NAMEFUNCT && !act->token->isDefine)
         {
            return false;
         }
         act = act->next;
      }
   }
   
   return true;
}

/*
 * Funkce, ktera prohleda cely seznam TS a v kazde TS hleda zadany
 * token.
 * @param table Seznam tabulek.
 * @param token Token, ktery se hleda.
 * @return Ukazatel na nalezenou polozku, jinak NULL.
 */
ThtableItem * searchTableList(TlistHtable *table, TToken *token)
{
   TListHtableItem *first = table->first;
   ThtableItem *found = NULL;
   while(first != table->act)
   {
      found = htable_search(first->table, token);
      if(found != NULL)
         return found;
      first = first->rptr;
   }
   
   return NULL;
}

/*
 * Vstupni funkce syn. analyzatoru.
 * @param Kod chyby.
 */
int parser(void)
{
   stackFor = malloc(sizeof(STACKFOR));
   if(stackFor == NULL) return INTERN_ERROR;
   stackForInit(stackFor);
   
   int synResult = SYNTAX_ERROR;
   if((actToken = get_token()) == NULL)
   {
      synResult = LEX_ERROR;
   }
   else
   {
      synResult = program();
   }
   addInstruction(g_instr, I_END, NULL, NULL, NULL);
   
   stackForClear(stackFor);
   free(stackFor);
   
   #ifndef PARSER_DEBUG
   /* Projdu celou tabulku symbolu a najdu nedefinovane volani funkci. */
   if(!searchUndefinedFunctions(ActualTable))
      return SEM_ERROR;
   #endif
   
   return synResult;
}

/*
 * Implementace gramatickeho pravidla <PROGRAM>
 * @return Kod chyby.
 */
int program(void)
{
   int synResult = SYNTAX_OK;
   if(actToken->type != _MAIN)
      return SYNTAX_ERROR;
   
   GetToken(actToken);
   synResult = listStatement();
   if(synResult != SYNTAX_OK)
      return synResult;
   
   if(actToken->type != _EOF)
      return SYNTAX_ERROR;
   
   return synResult;
}

/*
 * Implementace gramatickeho pravidla <LST>
 * @return Kod chyby.
 */
int listStatement(void)
{
   int synResult = SYNTAX_OK;
   //actToken = get_token();
   TToken *funcCopy = NULL;
   TToken *label = NULL, *labelJump = NULL;

   switch(actToken->type)
   {
      /* Pravidlo <LST> -> FUNCTION id(<PARAMS>) { <BODY> } <LST> */
      case _FUNCT:
         
         GetToken(actToken);
         if(actToken->type != _NAMEFUNCT)
            return SYNTAX_ERROR;
           
         funcCopy = actToken;
         #ifdef DPRINT
         printf("deklarace funkce: %s\n", actToken->name);  
         #endif 
         
         labelJump = createLabel();
         if(labelJump == NULL)   return INTERN_ERROR;
         addInstruction(g_instr, I_JUMP, NULL, NULL, labelJump);
         label = malloc(sizeof(TToken));
         if(label == NULL)   return INTERN_ERROR;
         label->name = NULL;
         label->value.stringVal = actToken->name;
         label->type = -1;
         if(token_list_insert(g_token_list, label) == NULL)
         {
            free((void *)label);
            return INTERN_ERROR;
         }
         
         if(label == NULL)   return INTERN_ERROR;
         addInstruction(g_instr, I_LABEL, label, NULL, NULL);
         
         GetToken(actToken);
         if(actToken->type != _LPARENTHESE)
            return SYNTAX_ERROR;
         
         synResult = params();
         if(synResult != SYNTAX_OK)
            return synResult;
         
         if(actToken->type != _RPARENTHESE)
            return SYNTAX_ERROR;
            
         addInstruction(g_instr, I_DELETE, NULL, NULL, NULL);
         
         #ifndef PARSER_DEBUG
         if((synResult = semFuncDeclaration(ActualTable, funcCopy, paramsCount)) != EOK)
            return synResult;
         #endif
         
         GetToken(actToken);
         if(actToken->type != _LBRACE)
            return SYNTAX_ERROR;
         
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
 
         if(actToken->type != _RBRACE)
            return SYNTAX_ERROR;
      
         addInstruction(g_instr, I_RETURN, NULL, NULL, NULL);
         addInstruction(g_instr, I_LABEL, labelJump, NULL, NULL);
       
         GetToken(actToken);
         synResult = listStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
         
         break;
         
      /* Pravidlo <LST> -> eps */
      case _EOF:

         return SYNTAX_OK;
         break;
      
      /* Pravidlo <LST> -> <BODY> <LST> */
      default:
         synResult = bodyStatement();

         if(synResult != SYNTAX_OK)
            return synResult;
         
         //if(actToken->type != _SEMICOLON)
         //   return SYNTAX_ERROR;
         
         /* Prijme i prazdne prikazy. */
         if(actToken->type == _SEMICOLON)
            GetToken(actToken);
         if(actToken->type == _RBRACE)
            return SYNTAX_ERROR;
         
         synResult = listStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
            
         break;
   }
   
   return synResult;
}

/*
 * Implementace gramatickeho pravidla <BODY>
 * @return Kod chyby.
 */
int bodyStatement(void)
{
   int synResult = SYNTAX_OK;
   TToken *funcCopy = NULL;
   TToken *outInstr  = NULL;
   
   TToken *labelBegin = NULL, *labelEnd = NULL, *labelContinue = NULL;

   switch(actToken->type)
   {
      /* Pravidlo <BODY> -> ID = <ASSIGN>; <BODY> */
      case _VARIABLE:
		
         #ifdef DPRINT
		   printf("deklarace promenne: %s\n", actToken->name); 
         #endif  
         funcCopy = actToken;
         
         GetToken(actToken);
         if(actToken->type != _EQUAL)
            return SYNTAX_ERROR;
         
         synResult = assign(&outInstr);
         if(synResult != SYNTAX_OK)
            return synResult;
      
         addInstruction(g_instr, I_ASSIGN, outInstr, NULL, 
            funcCopy);
         
         if(actToken->type != _SEMICOLON)
            return SYNTAX_ERROR;   

         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
         break;
        
      /* Pravidlo <BODY> -> WHILE (<EXPR>) { <BODY> } <BODY> */
      case _WHILE:
         #ifdef DPRINT
         printf("while\n");   
         #endif
         GetToken(actToken);
         if(actToken->type != _LPARENTHESE)
            return SYNTAX_ERROR;
            
         labelBegin = createLabel();
         if(labelBegin == NULL)   return INTERN_ERROR;
         addInstruction(g_instr, I_LABEL, labelBegin, NULL, NULL);
         
         synResult = evaluateExpression(actToken, &actToken, NULL); /* Dalsi token jiz nacten. */
         if(synResult != SYNTAX_OK)
            return synResult;
         if(AddTemporaryVariable(outInstr) != SYNTAX_OK) return INTERN_ERROR;
            
         if(actToken->type != _LBRACE)
            return SYNTAX_ERROR;
            
         labelEnd = createLabel();
         if(labelEnd == NULL)   return INTERN_ERROR;
         addInstruction(g_instr, I_COND_JUMP, NULL, NULL, labelEnd);
    
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;

         if(actToken->type != _RBRACE)
            return SYNTAX_ERROR;
            
         addInstruction(g_instr, I_JUMP, NULL, NULL, labelBegin);
         addInstruction(g_instr, I_LABEL, labelEnd, NULL, NULL);
        
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
         break;
      
      /* Pravidlo <BODY> -> RETURN <EXPR>; <BODY> */   
      case _RETURN:
         #ifdef DPRINT
         printf("return: %d\n", actToken->type);   
         #endif
         
         GetToken(actToken);         
         synResult = evaluateExpression(actToken, &actToken, &outInstr);  /* Dalsi token jiz nacten. */
         if(synResult != SYNTAX_OK)
            return synResult;

         TToken *ret = malloc(sizeof(TToken));
         if(ret == NULL)
            return INTERN_ERROR;
         if(token_list_insert(g_token_list, ret) == NULL)
         {
            free((void *)ret);
            return INTERN_ERROR;
         }
            
         ret->name = createTmpVarName();
         if(ret->name == NULL)   return INTERN_ERROR;
         ret->type = _VARIABLE;
         
         addInstruction(g_instr, I_ASSIGN, outInstr, NULL, 
            ret);
         addInstruction(g_instr, I_RETURN, ret, NULL, &g_return);
         
            
         if(actToken->type != _SEMICOLON)
            return SYNTAX_ERROR;
 
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
         break;
      
      /* Pravidlo <BODY> -> IF (<EXPR>) { <BODY> } else { <BODY> } <BODY> */   
      case _IF:
         #ifdef DPRINT
         printf("if\n");
         #endif
         GetToken(actToken);
         if(actToken->type != _LPARENTHESE)
            return SYNTAX_ERROR;
            
         synResult = evaluateExpression(actToken, &actToken, &outInstr); /* Dalsi token jiz nacten. */
         if(synResult != SYNTAX_OK)
            return synResult;
         if(AddTemporaryVariable(outInstr) != SYNTAX_OK) return INTERN_ERROR;
            
         if(actToken->type != _LBRACE)
            return SYNTAX_ERROR;
            
         /* Vlozime instrukci pro if */
         labelBegin = createLabel();
         if(labelBegin == NULL)   return INTERN_ERROR;
         addInstruction(g_instr, I_COND_JUMP, NULL, NULL, labelBegin);
           
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;

         if(actToken->type != _RBRACE)
            return SYNTAX_ERROR;
           
         GetToken(actToken);
         if(actToken->type != _ELSE)
            return SYNTAX_ERROR;
         GetToken(actToken);
         if(actToken->type != _LBRACE)
            return SYNTAX_ERROR;
         #ifdef DPRINT
         printf("else: %d\n", actToken->type);   
         #endif
         
         labelEnd = createLabel();
         if(labelEnd == NULL)   return INTERN_ERROR;
         addInstruction(g_instr, I_JUMP, NULL, NULL, labelEnd);
         addInstruction(g_instr, I_LABEL, labelBegin, NULL, NULL);
         
         GetToken(actToken);   
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
          
         if(actToken->type != _RBRACE)
            return SYNTAX_ERROR;
            
         addInstruction(g_instr, I_LABEL, labelEnd, NULL, NULL);
         
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
         break;
       
      /* Pravidlo <BODY> -> _FUNCNAME(<ARGS>); <BODY> */
      /*case _NAMEFUNCT:
         
         funcCopy = actToken;
         #ifdef DPRINT
         printf("volani funkce: %s\n", actToken->name);
         #endif
         GetToken(actToken);
         if(actToken->type != _LPARENTHESE)
            return SYNTAX_ERROR;
         
         synResult = args(); //Vyhodnoceni vyrazu...
         if(synResult != SYNTAX_OK)
            return synResult;
   
         if(actToken->type != _RPARENTHESE)
            return SYNTAX_ERROR;
         
         GetToken(actToken);
         if(actToken->type != _SEMICOLON)
            return SYNTAX_ERROR;
            
         //addInstruction(instrList, I_JUMPFCE, funcCopy, NULL, NULL);
         callFunctionInstruction(g_instr, funcCopy);
         
         if(semFuncCall(ActualTable, funcCopy, paramsCount) != EOK)
            return SEM_ERROR;  
            
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
         break;*/
      
      /* Pravidlo <BODY> -> eps */   
      
      /* Pravidlo pro continue. */
      case _CONTINUE:
         
         if(stackForEmpty(stackFor))
            return SYNTAX_ERROR;
         
         addInstruction(g_instr, I_JUMP, NULL, NULL, 
            stackForTop(stackFor)->labelContinue);   
         
         GetToken(actToken);
         if(actToken->type != _SEMICOLON)
            return SYNTAX_ERROR;
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
         break;
      
      /* Pravidlo pro break. */
      case _BREAK:
         
         if(stackForEmpty(stackFor))
            return SYNTAX_ERROR;
            
         addInstruction(g_instr, I_JUMP, NULL, NULL, 
            stackForTop(stackFor)->labelBreak);
      
         GetToken(actToken);
         if(actToken->type != _SEMICOLON)
            return SYNTAX_ERROR;
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
         break;
      
      /* Pravidlo pro for */
      case _FOR:
         
         GetToken(actToken);
         if(actToken->type != _LPARENTHESE)
            return SYNTAX_ERROR;
         
         GetToken(actToken);
         synResult = forAssign(0);
         if(synResult != SYNTAX_OK)
            return synResult;
         if(actToken->type != _SEMICOLON)
            return SYNTAX_ERROR;
           
         /* Label pro zacatek cyklu. */   
         labelBegin = createLabel();
         if(labelBegin == NULL)   return INTERN_ERROR;
         addInstruction(g_instr, I_LABEL, labelBegin, NULL, NULL);
         /* Label pro skok break. */
         labelEnd = createLabel();
         if(labelEnd == NULL)   return INTERN_ERROR;
         /* Label pro skok continue. */
         labelContinue = createLabel();
         if(labelContinue == NULL)   return INTERN_ERROR;
         
         stackForPush(stackFor, labelEnd, labelContinue);
       
         GetToken(actToken);
         if(actToken->type != _SEMICOLON)
         {
            synResult = evaluateExpression(actToken, &actToken, NULL); /* Dalsi token jiz nacten. */
            if(synResult != SYNTAX_OK)
               return synResult;
            if(AddTemporaryVariable(outInstr) != SYNTAX_OK) return INTERN_ERROR;
            
            addInstruction(g_instr, I_COND_JUMP, NULL, NULL, labelEnd);
         }
         
         if(actToken->type != _SEMICOLON)
            return SYNTAX_ERROR;
         
         TinstrItem * lst = g_instr->last;
         
         addInstruction(g_instr, I_LABEL, labelContinue, NULL, NULL); 
         GetToken(actToken);
         if(actToken->type != _RPARENTHESE)
         {
            synResult = forAssign(1);
            if(synResult != SYNTAX_OK)
               return synResult;
         }
         else
         {
            GetToken(actToken);
         }
         if(actToken->type != _LBRACE)
            return SYNTAX_ERROR;
         
         TinstrItem *lst2 = g_instr->last;
         
         /* Telo cyklu */
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;

         if(actToken->type != _RBRACE)
            return SYNTAX_ERROR;
         
         /* Prehozeni casti instrukci kvuli continue. */
         TinstrItem * tmp = lst->rptr;
         if(tmp != NULL && lst2->rptr != NULL)
         {
            lst->rptr = lst2->rptr;
            lst2->rptr->lptr = lst;
            g_instr->last->rptr = tmp;
            tmp->lptr = g_instr->last;
            lst2->rptr =  NULL;
            g_instr->last = lst2;
         }
         free(stackForPop(stackFor));
         addInstruction(g_instr, I_JUMP, NULL, NULL, labelBegin);
         addInstruction(g_instr, I_LABEL, labelEnd, NULL, NULL);
        
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;
   
         break;
         
      
      case _SEMICOLON: /* Prazdny prikaz. */
         GetToken(actToken);
         synResult = bodyStatement();
         if(synResult != SYNTAX_OK)
            return synResult;

         break;
      
      default:
         if(actToken->type == _SEMICOLON || actToken->type == _RBRACE || 
            actToken->type == _EOF || actToken->type == _FUNCT)
         {
            return SYNTAX_OK;
         }
         else
            return SYNTAX_ERROR;
   }
   
   return synResult;
}

/*
 * Implementace gramatickeho pravidla <PARAMS>
 * @return Kod chyby.
 */
int params(void)
{
   int synResult = SYNTAX_OK;
   paramsCount = 0;
   
   synResult = parFirst();
   if(synResult != SYNTAX_OK)
      return synResult;
      
   synResult = parNext();
   if(synResult != SYNTAX_OK)
      return synResult;
      
   return synResult;
}

/*
 * Implementace gramatickeho pravidla <PARFIRST>
 * @return Kod chyby.
 */
int parFirst(void)
{
   //int synResult = SYNTAX_OK;
   GetToken(actToken);
   
   if(actToken->type == _VARIABLE)
   {
      paramsCount++;
      
      addInstruction(g_instr, I_INSERT, NULL, NULL, actToken);
      
      //GetToken(actToken);
      return SYNTAX_OK;
   }
   else if(actToken->type == _RPARENTHESE)
   {
      return SYNTAX_OK;
   }
   else
   {
      return SYNTAX_ERROR;
   }
}

/*
 * Implementace gramatickeho pravidla <PARNEXT>
 * @return Kod chyby.
 */
int parNext(void)
{
   int synResult = SYNTAX_OK;
   if(actToken->type == _RPARENTHESE)
      return SYNTAX_OK;
   GetToken(actToken);   

   if(actToken->type == _COMMA)
   {
      paramsCount++;
      
      GetToken(actToken);
      if(actToken->type != _VARIABLE)
         return SYNTAX_ERROR;
      
      addInstruction(g_instr, I_INSERT, NULL, NULL, actToken);
      
      //GetToken(actToken);
      synResult = parNext();
      if(synResult != SYNTAX_OK)
         return synResult;
   }
   else if(actToken->type == _RPARENTHESE)
   {
      return SYNTAX_OK;
   }
   else
   {
      return SYNTAX_ERROR;
   }
   
   return synResult;
}

/*
 * Implementace gramatickeho pravidla <ASSIGN>
 * @return Kod chyby.
 */
int assign(TToken **outInstr)
{
   int result = SYNTAX_OK;
   TToken *funcCopy = NULL;
   GetToken(actToken);
   
   /* Pravidlo <ASSIGN> -> _NAMEFUNC(<ARGS>) */
   if(actToken->type == _NAMEFUNCT)
   { 
      funcCopy = actToken;
      #ifdef DPRINT
      printf("volani funkce: %s\n", actToken->name);
      #endif
      GetToken(actToken);
      if(actToken->type != _LPARENTHESE)
         return SYNTAX_ERROR;
      result = args();
      if(result != SYNTAX_OK)
         return result;
      if(actToken->type != _RPARENTHESE)
         return SYNTAX_ERROR;
      
      if(callFunctionInstruction(g_instr, funcCopy) != SYNTAX_OK)
         return INTERN_ERROR;
      *outInstr = &g_return;
      
      if(semFuncCall(ActualTable, funcCopy, paramsCount) != EOK)
         return PARAMS_ERROR;
         
      GetToken(actToken);
      
      return SYNTAX_OK;
   }
   /* Pravidlo <ASSIGN> -> <EXPR> */
   else
   {
      *outInstr = actToken;
      result = evaluateExpression(actToken, 
         &actToken, outInstr);

      if(result != SYNTAX_OK)
         return result;
      /* Nacte token i za <EXPR> */
   }
   return SYNTAX_OK;
}

/*
 * Implementace gramatickeho pravidla <VALUE>
 * @return Kod chyby.
 */
int value(void)
{
   if(actToken->type == _INTVALUE || actToken->type == _DOUBLEVALUE || 
      actToken->type == _STRINGVALUE || actToken->type == _BOOLVALUE || 
      actToken->type == _NULLVALUE)
   {
      return SYNTAX_OK;
   }
   return SYNTAX_ERROR;
}

/*
 * Implementace gramatickeho pravidla <ARGS>
 * @return Kod chyby.
 */
int args(void)
{
   paramsCount = 0;
   int synResult = SYNTAX_OK;

   synResult = argFirst();
   if(synResult != SYNTAX_OK)
      return synResult;
      
   synResult = argNext();
   if(synResult != SYNTAX_OK)
      return synResult;
      
   return synResult;
}

/*
 * Implementace gramatickeho pravidla <ARGFIRST>
 * @return Kod chyby.
 */
int argFirst(void)
{
   //int synResult = SYNTAX_OK;
   GetToken(actToken);
   
   if(actToken->type == _VARIABLE)
   {
      paramsCount++;
      addInstruction(g_instr, I_PUSH, actToken, NULL, NULL);
      //GetToken(actToken);
      return SYNTAX_OK;
   }
   else if(actToken->type == _RPARENTHESE)
   {
      return SYNTAX_OK;
   }
   else /* Pravidlo <ARGFIRST> -> VALUE */
   {
      paramsCount++;
      addInstruction(g_instr, I_PUSH, actToken, NULL, NULL);
      //GetToken(actToken);
      return value();
   }
}

/*
 * Implementace gramatickeho pravidla <ARGNEXT>
 * @return Kod chyby.
 */
int argNext(void)
{
   int synResult = SYNTAX_ERROR;

   /* Jedna se o EPS pravidlo */
   if(actToken->type == _RPARENTHESE)
      return SYNTAX_OK;
   GetToken(actToken);
   
   if(actToken->type == _COMMA)
   {
      GetToken(actToken);
      if(actToken->type == _VARIABLE)
      {
         paramsCount++;
         
         addInstruction(g_instr, I_PUSH, actToken, NULL, NULL);

         synResult = argNext();
         if(synResult != SYNTAX_OK)
            return synResult;
      }
      else
      {
         paramsCount++;
         synResult = value();
         if(synResult != SYNTAX_OK)
            return synResult;
         
         addInstruction(g_instr, I_PUSH, actToken, NULL, NULL);
         
         synResult = argNext();
         if(synResult != SYNTAX_OK)
            return synResult;
      }
      
   }
   else if(actToken->type == _RPARENTHESE)
   {
      return SYNTAX_OK;
   }
   else
   {
      return SYNTAX_ERROR;
   }
   
   return synResult;
}

/*
 * Funkce, ktera implementuje prirazeni v cyklu for.
 * @param prev Jedna se o 1. nebo 3. cast cyklu for.
 * @return Uspesnost.
 */
int forAssign(int prev)
{
   TToken *outInstr  = NULL;
   TToken *funcCopy = NULL;
   int result = SYNTAX_ERROR;
   
   if(actToken->type == _SEMICOLON)
      return SYNTAX_OK;
   if(actToken->type != _VARIABLE)
      return SYNTAX_ERROR;
   funcCopy = actToken;
   
   GetToken(actToken);
   if(actToken->type != _EQUAL)
      return SYNTAX_ERROR;
   
   if(prev == 0)
   {
      GetToken(actToken);
   }
   else
      actToken->type = _LPARENTHESE;
    
   outInstr = actToken;
   result = evaluateExpression(actToken, 
         &actToken, &outInstr);
   
   if(result != SYNTAX_OK)
      return result;
       
   addInstruction(g_instr, I_ASSIGN, outInstr, NULL, 
      funcCopy);
         
   return SYNTAX_OK;
}
