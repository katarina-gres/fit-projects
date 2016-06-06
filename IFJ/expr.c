// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek
 
/* 
 * Implementace syntaktické analýzy ze zdola nahoru.
 */

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "ial.h"
#include "expr.h"
#include "adt.h"
#include "generator.h"
#include "global.h"

/* Precedencni tabulka pro kontroly syntaxe vyrazu. */
const int PrecedenceTable[TABLESIZE][TABLESIZE] = {
      /*  +    -    *    /    ID   <    >    .   ===  !==   (    )    <=   >=   $   */  
/* + */ {'>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '<', '>', '>', '>', '>'},
/* - */ {'>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '<', '>', '>', '>', '>'},
/* * */ {'>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '<', '>', '>', '>', '>'},
/* / */ {'>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '<', '>', '>', '>', '>'},
/*ID */ {'>', '>', '>', '>',  0 , '>', '>', '>', '>', '>',  0 , '>', '>', '>', '>'},
/* < */ {'<', '<', '<', '<', '<', '>', '>', '<', '>', '>', '<', '>', '>', '>', '>'},
/* > */ {'<', '<', '<', '<', '<', '>', '>', '<', '>', '>', '<', '>', '>', '>', '>'},
/* . */ {'>', '>', '<', '<', '<', '>', '>', '>', '>', '>', '<', '>', '>', '>', '>'},
/*===*/ {'<', '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '<', '<', '>'},
/* !==*/{'<', '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '<', '<', '>'},
/* ( */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', '<',  0 },
/* ) */ {'>', '>', '>', '>',  0 , '>', '>', '>', '>', '>',  0 , '>', '>', '>', '>'},
/* <=*/ {'<', '<', '<', '<', '<', '>', '>', '<', '>', '>', '<', '>', '>', '>', '>'},
/* >=*/ {'<', '<', '<', '<', '<', '>', '>', '<', '>', '>', '<', '>', '>', '>', '>'},
/* $ */ {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<',  0 , '<', '<',  DOLLAR }
};

const int Operands[11][5] = {
   /* null bool string int double */
   {0, 0, 0, 1, 1}, /* + */
   {0, 0, 0, 1, 1}, /* - */
   {0, 0, 0, 1, 1}, /* * */
   {0, 0, 0, 1, 1}, /* / */
   {1, 1, 1, 1, 1}, /* . */ /* Alespon 1 operand musi byt string */
   {1, 1, 1, 1, 1}, /* === */
   {1, 1, 1, 1, 1}, /* !== */
   {1, 1, 1, 1, 1}, /* <= */
   {1, 1, 1, 1, 1}, /* >= */
   {1, 1, 1, 1, 1}, /* < */
   {1, 1, 1, 1, 1}, /* < */
};

/*
 * Funkce pro inicializaci zasoniku.
 * @param stack Zasobnik, ktery se ma inicializovat.
 */
void stackInit(STACK *stack)
{
   stack->top = NULL;
}

/*
 * Funkce pro vyjmuti posledniho prvku ze zasobniku.
 * @param stack Zasobnik, ze ktereho se ma vyjmout hodnota.
 * @return Vyjmuta data.
 */
STACKDATA *stackPop(STACK *stack)
{
   if(stack->top == NULL)
      return NULL;
      
   STACKDATA *ret = stack->top;
   stack->top = stack->top->next;
   
   return ret;
}

/*
 * Vlozeni hodnoty na vrchol zasobniku.
 * @param stack Zasobnik na jehoz vrchol se maji vlozit data.
 * @param value Hodnota, ktera se ma vlozit.
 * @return Uspesnost operace.
 */
int stackPush(STACK *stack, int value, TToken *token)
{
   STACKDATA *ins = malloc(sizeof(STACKDATA));
   if(ins == NULL)
      return STERROR;
   ins->data = value;  
   ins->next = NULL; 
   ins->token = token;
      
   if(stack->top == NULL)
      stack->top = ins;
   else
   {
      ins->next = stack->top;
      stack->top = ins;
   }
   
   return STOK;
}

/*
 * Kontrola prazdnosti zasobniku (true = prazdny).
 * @param stack Kontrolovany zasobnik.
 */
int stackEmpty(STACK *stack)
{
   return (stack->top == NULL);
}

/*
 * Funkce pro vymazani celeho zasobniku.
 * @param stack Zasobnik, ktery se ma vymazat.
 */
void stackClear(STACK *stack)
{
   STACKDATA *act = stack->top;
   while(stack->top != NULL)
   {
      act = stack->top;
      stack->top = stack->top->next;
      free((void *)act);
   }
}

/*
 * Funkce, pro smazani vsech pouzivanych dynamickych datovych struktur.
 * @param stack Zasobnik pro smazani.
 */
void freeDataStructures(STACK *stack)
{
   if(stack != NULL)
   {
      stackClear(stack);
   }
}

/*
 * Funkce, pro vyjmuti nejvyssiho terminalu ze zasobniku.
 * @param stack Zasobnik, ze ktereho se ma vyjmout.
 */
STACKDATA* topStackTerminal(STACK *stack)
{
   STACKDATA *top = stack->top;
   while(top != NULL && top->data == ENOTERM)
   {
      top = top->next;
   }
   
   return top;
}

/*
 * Funkce, ktera prevede typ tokenu na index do precedencni tabulky.
 * @param con Typ tokenu, ktery se ma prevest.
 * @return Index do tabulky symbolu.
 */
int convertIndex(int con)
{
   if(con >= 19 && con <= 24)
      return 4;
   switch(con)
   {
      case _PLUS:
         return 0;
      case _MINUS:
         return 1;
      case _MULTI:
         return 2;
      case _DIV:
         return 3;
      case _LESS:
         return 5;
      case _GREATER:
         return 6;
      case _DOT:
         return 7;
      case _3EQ:
         return 8;
      case _N3EQ:
         return 9;
      case _LPARENTHESE:
         return 10;
      case _RPARENTHESE:
         return 11;
      case _LEQ:
         return 12;
      case _GEQ:
         return 13;
         
      case _LBRACE:
      case _SEMICOLON:
      case DOLLAR:
         return 14;
         
      default:
         return -1;
   }
}

/*
 * Funkce, ktera provede kontrolu (syn., sem.) vyrazu.
 * @param Seznam tabulek symbolu.
 * @param actToken Aktualne nacteny token.
 * @param lastToken Posledni zpracovany token.
 * @return Uspesnost vyhodnoceni.
 */
int evaluateExpression(TToken *actToken, TToken **lastToken, 
   TToken **outInstr)
{
   int topTerminal;
   int tableData;
   TToken* actInput = actToken;
   STACKDATA *top = NULL;
   STACK stack;
   int error = SYNTAX_ERROR;
   
   if(actInput == NULL || IsEndSymbol(actInput->type))
   {
      return SYNTAX_ERROR;
   }
   
   stackInit(&stack);
   stackPush(&stack, DOLLAR, NULL);
   if(outInstr != NULL) *outInstr = actToken;

   do
   {
      top = topStackTerminal(&stack);
      if(top == NULL)
      {
         freeDataStructures(&stack);
         return SYNTAX_ERROR;
      }
      topTerminal = top->data;
      
      int index1 = convertIndex(topTerminal);
      int index2 = convertIndex(actInput->type);

      if(index1 == -1 || index2 == -1)
      {
         freeDataStructures(&stack);
         return SYNTAX_ERROR;
      }
      tableData = SATable(index1, index2);
      
      switch(tableData)
      {
         case '=':
         case '<':
            pushToken(&stack, actInput);
            
            actInput = get_token();
            if(actInput == NULL)
            {
               freeDataStructures(&stack);
               return SYNTAX_ERROR;
            }
            *lastToken = actInput;

            break;
            
         case '>':
            error = replaceRule(&stack, outInstr);
            if(error != SYNTAX_OK)
            {
               freeDataStructures(&stack);
               return error;
            }
            break;
            
         case DOLLAR:
            /* Konec */       
            if(stackEmpty(&stack))
            {
               freeDataStructures(&stack);
               return SYNTAX_ERROR;
            }
            if((top = stackPop(&stack))->data != ENOTERM)
            {
               free((void *)top);
               freeDataStructures(&stack);
               return SYNTAX_ERROR;
            }
            
            free((void *)top);
            freeDataStructures(&stack);
            return SYNTAX_OK;
            
         default:
            freeDataStructures(&stack);
            return SYNTAX_ERROR;
         
      }
   } while(!IsEndSymbol(topTerminal) || !IsEndSymbol(actInput->type));
   
   freeDataStructures(&stack);
   return SYNTAX_OK;
}

/*
 * Funkce, ktera vytiskne obsah zasobniku.
 * @param stack Zasobnik, ktery se ma vytisknout.
 */
void printStack(STACK *stack)
{
   printf("----------\n");
   STACKDATA *tmp = stack->top;
   while(tmp != NULL)
   {
      printf("%d\n", tmp->data);
      tmp = tmp->next;
   }
   putchar('\n');
}


/*
 * Funkce, ktera nahradi pravidlo z vrcholu zasobniku.
 * @param stack Zasobnik.
 * @param outInstr Docasna promenna do ktere je ulozen vysledek operace.
 * @return Uspesnost operace.
 */
int replaceRule(STACK *stack, TToken **outInstr)
{
   int res = SYNTAX_ERROR;
   STACKDATA *top = NULL;
   TToken *tmpOp1, *tmpOp2, *operator = NULL;
   
   if(stackEmpty(stack))
      return SYNTAX_ERROR;
      
   if((top = stackPop(stack)) != NULL)
   {
      /* Pravidlo E -> id */
      if(top->data == _VARIABLE)
      {
         res = SYNTAX_OK;
         if(outInstr != NULL) *outInstr = top->token;
         stackPush(stack, ENOTERM, top->token);
      }
      /* Pravidlo E -> const */
      else if(IsConstant(top->data))
      {
         res = SYNTAX_OK;
         if(outInstr != NULL) *outInstr = top->token;
         stackPush(stack, ENOTERM, top->token);
      }
      /* Pravidlo E -> (E) */
      else if(top->data == _RPARENTHESE)
      {
         free((void *)top);
         if((top = stackPop(stack)) != NULL && 
            top->data == ENOTERM)
         {
            tmpOp1 = top->token;
            free((void *)top);
            if((top = stackPop(stack)) != NULL && 
               top->data == _LPARENTHESE)
            {
               res = SYNTAX_OK;
               stackPush(stack, ENOTERM, tmpOp1);
            }
         }
      }
      /* Pravidlo E -> E op E */
      else if(top->data == ENOTERM)
      {
         tmpOp1 = top->token;
         free((void *)top);
         if((top = stackPop(stack)) != NULL && 
            IsOperator(top->data))
         {
            operator = top->token;
            free((void *)top);
            if((top = stackPop(stack)) != NULL && 
               top->data == ENOTERM)
            {
               tmpOp2 = top->token;
               Tinstr *ins = createArithmeticInstruction(tmpOp2, tmpOp1, operator);
               if(outInstr != NULL) *outInstr = (TToken *)ins->adr3;
               
               res = SYNTAX_OK;
               stackPush(stack, ENOTERM, (TToken *)ins->adr3);
               instr_list_insert_last(ins, g_instr);
            }
         }
      }
      free((void *)top);
   }
   
   return res;
}

/*
 * Funkce, ktera vlozi token na vrchol zasobniku.
 * @param stack Zasobnik.
 * @param token Token, ktery se ma vlozit.
 */
int pushToken(STACK *stack, TToken * token)
{
   if(!stackPush(stack, token->type, token))
      return SYNTAX_ERROR;
   
   return SYNTAX_OK;
}

