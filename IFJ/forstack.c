// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

/*
 * Implementace zasobniku pro uchovavani labelu 
 * pro BREAK, CONTINUE.
 */

#include "forstack.h"
#include "expr.h"

/* 
 * Funkce pro inicializaci zasobniku. 
 * @param stack Zasobnik pro inicializaci.
 */
void stackForInit(STACKFOR *stack)
{
   stack->top = NULL;
}

/*
 * Funkce pro vyjmuti posledniho prvku ze zasobniku.
 * @param stack Zasobnik, ze ktereho se ma vyjmout hodnota.
 * @return Vyjmuta data.
 */
STACKFORDATA *stackForPop(STACKFOR *stack)
{
   if(stack->top == NULL)
      return NULL;
      
   STACKFORDATA *ret = stack->top;
   stack->top = stack->top->next;
   
   return ret;
}

/*
 * Vlozeni hodnoty na vrchol zasobniku.
 * @param stack Zasobnik na jehoz vrchol se maji vlozit data.
 * @param value Hodnota, ktera se ma vlozit.
 * @return Uspesnost operace.
 */
int stackForPush(STACKFOR *stack, TToken *labelBreak, TToken *labelContinue)
{
   STACKFORDATA *ins = malloc(sizeof(STACKFORDATA));
   if(ins == NULL)
      return STERROR; 
   ins->next = NULL; 
   ins->labelBreak = labelBreak;
   ins->labelContinue = labelContinue;
      
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
int stackForEmpty(STACKFOR *stack)
{
   return (stack->top == NULL);
}

/*
 * Funkce pro vymazani celeho zasobniku.
 * @param stack Zasobnik, ktery se ma vymazat.
 */
void stackForClear(STACKFOR *stack)
{
   STACKFORDATA *act = stack->top;
   while(stack->top != NULL)
   {
      act = stack->top;
      stack->top = stack->top->next;
      free((void *)act);
   }
}

/*
 * Funkce pro zjisteni obsahu vrcholu zasobniku.
 * @param stack Zasobnik
 * @return Vrchol zasobniku.
 */
STACKFORDATA * stackForTop(STACKFOR *stack)
{
   if(stack->top == NULL)
      return NULL;
      
   return stack->top;
}
