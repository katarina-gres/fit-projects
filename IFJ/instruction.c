// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"

/**
 * Vytvoreni seznamu instrukci a inicializace
 * @return ukazatel na seznam
 */
TlistInstr * instr_list_init(){
	TlistInstr * list = malloc(sizeof(TlistInstr));
	if(list == NULL)
		return NULL;
	list->first = NULL;
	list->act = NULL;
	list->last = NULL;

	return list;
}

/**
 * zda je seznam prazdny
 * @param  list testovany seznam
 * @return      prazdny 1 / jinak 0
 */
int instr_list_empty(TlistInstr * list){
	return (list->first == NULL ? 1 : 0);
}

/**
 * vlozi instrukci na konec seznamu
 * @param  instr ukazatel na instrukci
 * @param  list  seznam do ktereho se vklada
 * @return       vlozena instrukce / NULL
 */
TinstrItem * instr_list_insert_last(Tinstr * instr,TlistInstr *list){
	TinstrItem * item = malloc(sizeof(TlistInstr));
	if(item == NULL)
		return NULL;
	item->instruction = instr;
	item->lptr = NULL;
	item->rptr = NULL;
	if(instr_list_empty(list)) // pokud je prazdy vlozim jako jediny
	{
		list->first = item;
		list->last = item;
	}
	else	// jinak ulozim nakonec
	{
		list->last->rptr = item;
		item->lptr = list->last;
		list->last = item;
	}

	return item;
}

/**
 * aktualni ukazuje na prvni
 * @param list seznam instrukci
 */
void instr_list_first(TlistInstr * list){
	list->act = list->first;
}

/**
 * posune ukazatel na dalsi
 * @param list seznam instrukci
 */
void instr_list_next(TlistInstr * list){
	if(list->act != NULL)
		list->act = list->act->rptr;
}



/**
 * nastaveni aktualni polozky na zadanou parametrem 
 * @param list seznam instrukci
 * @param item pozadovana polozka
 */
void instr_list_goto(TlistInstr * list, TinstrItem * item){
	if(list != NULL)
		list->act = item;
}
/**
 * vrati dalsi instrukci v seznamu
 * @param  list seznam
 * @return      ukazatel na dalsi polozku
 */
TinstrItem * intr_list_get_next(TlistInstr * list){
	if(list == NULL)
		return NULL;
	if(list->act == NULL)
		return NULL;
	return list->act;
}
/**
 * uvolni seznam - neuvolnuje primo hodnoty, ty by měly byt uvolneny v 
 * tabulce symbolu
 * @param list seznam instrukci
 */
void instr_list_free(TlistInstr * list){
	TinstrItem * item = list->first;
	TinstrItem * temp = NULL;

	while(item != NULL)
	{
		temp = item->rptr;
		free(item->instruction);
		free(item);
		item = temp;
	}
	list->first = NULL;
	list->act = NULL;
	list->last = NULL;
	free(list);
}

/**
 * vrati act ukazatel na instrukci 
 * @param  list seznam instrukci
 * @return      ukazatel na instrukci / NULL
 */
Tinstr * instr_list_get_data(TlistInstr * list){
	if(list->act != NULL)
		return (list->act->instruction);
	return NULL;
}

/**
 * Inicializace zasobniku pro Iteminstrukce
 * @return - ukazatel na stack
 */
TinstrStack * instr_stack_init(){
    TinstrStack * stack = malloc(sizeof(TinstrStack));
    if(stack == NULL)
        return NULL;
    stack->top = NULL;
    return stack;
}

/**
 * Jestli je stack prazdny
 * @param TinstrStack * stack - zasobnik
 * @return - 1 prazdny / jinak 0
 */
int instr_stack_is_empty(TinstrStack * stack){
    return (stack->top == NULL ? 1 : 0);
}

/**
 * Push ItemInstrukce na stack
 * @param TinstrStack * stack - zasobnik
 * @param Tinstr * instr - instrukce
 */
void instr_stack_push(TinstrStack * stack, TinstrItem * instrItem){
    if(stack == NULL || instrItem == NULL)
        return;
    TinstrStackItem *item = malloc(sizeof(TinstrStackItem));
    if(item == NULL)
        return;
    item->instrItem = instrItem;
    item->next = stack->top;
    stack->top = item;
    return;
}

/**
 * Stack TOP
 * @param TinstrStack * stack - zasobniku
 * @return - ItemInstrukce - vrchol zasobniku
 */
TinstrItem * instr_stack_top(TinstrStack * stack){
    if(stack == NULL || instr_stack_is_empty(stack))
        return NULL;
    return stack->top->instrItem;
}

/**
 * Odebrani prvku z vrcholu zasobniku
 * @param TinstrStack * stack - zasobnik
 */
void instr_stack_pop(TinstrStack * stack){
    if(stack == NULL || instr_stack_is_empty(stack))
        return;
    TinstrStackItem * temp = stack->top;
    stack->top = temp->next;
    free(temp);
    return;
}

/**
 * Smaze vsechny polozky - stack zustane jako po inicializaci
 * @param TinstrStack * stack - zasobnik
 */
void instr_stack_clear_all(TinstrStack * stack){
    if(stack == NULL)
        return;
    while(!instr_stack_is_empty(stack))
        instr_stack_pop(stack);
}

/**
 * Smaze cely zasobnik
 * @param TinstrStack * stack - zasobnik
 */
void instr_stack_free(TinstrStack * stack){
    if(stack == NULL)
        return;
    instr_stack_clear_all(stack);
    free(stack);
    stack = NULL;
    return;
}
