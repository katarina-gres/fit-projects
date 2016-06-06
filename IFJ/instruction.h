// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#ifndef _INSTRUCT_H
#define _INSTRUCT_H

// struktura 3 adresneho kodu
typedef struct instr{
	int instr_type;
	void *adr1;
	void *adr2;
	void *adr3;
}Tinstr;

// struktura pro polozku v instrListu
typedef struct instrItem TinstrItem;

struct instrItem{
	Tinstr * instruction;
	TinstrItem *lptr;
	TinstrItem *rptr;
};
// struktura pro seznam instrukci
typedef struct 
{
	TinstrItem *first;
	TinstrItem *act;
	TinstrItem *last;
}TlistInstr;

typedef struct instrStackItem TinstrStackItem;

struct instrStackItem{
	TinstrItem * instrItem;
	TinstrStackItem * next;
};

typedef struct{
	TinstrStackItem * top;
}TinstrStack;

/*************************** seznam pro instrukce ***************************/
/**
 * Vytvoreni seznamu instrukci a inicializace
 * @return ukazatel na seznam
 */
TlistInstr * instr_list_init();

/**
 * zda je seznam prazdny
 * @param  list testovany seznam
 * @return      prazdny 1 / jinak 0
 */
int instr_list_empty(TlistInstr * list);

/**
 * vlozi instrukci na konec seznamu
 * @param  instr ukazatel na instrukci
 * @param  list  seznam do ktereho se vklada
 * @return       vlozena instrukce / NULL
 */
TinstrItem * instr_list_insert_last(Tinstr * instr,TlistInstr *list);

/**
 * aktualni ukazuje na prvni
 * @param list seznam instrukci
 */
void instr_list_first(TlistInstr * list);
/**
 * posune ukazatel na dalsi
 * @param list seznam instrukci
 */
void instr_list_next(TlistInstr * list);

/**
 * nastaveni aktualni polozky na zadanou parametrem 
 * @param list seznam instrukci
 * @param item pozadovana polozka
 */
void instr_list_goto(TlistInstr * list, TinstrItem * item);

/**
 * uvolni seznam - neuvolnuje primo hodnoty, ty by měly byt uvolneny v 
 * tabulce symbolu
 * @param list seznam instrukci
 */
void instr_list_free(TlistInstr * list);

/**
 * vrati act ukazatel na instrukci 
 * @param  list seznam instrukci
 * @return      ukazatel na instrukci / NULL
 */
Tinstr * instr_list_get_data(TlistInstr * list);

/**
 * vrati dalsi instrukci v seznamu
 * @param  list seznam
 * @return      ukazatel na dalsi polozku
 */
TinstrItem * intr_list_get_next(TlistInstr * list);

/*************************** Stack pro instrukce ***************************/
/**
 * Inicializace zasobniku pro Iteminstrukce
 * @return - ukazatel na stack
 */
TinstrStack * instr_stack_init();

/**
 * Jestli je stack prazdny
 * @param TinstrStack * stack - zasobnik
 * @return - 1 prazdny / jinak 0
 */
int instr_stack_is_empty(TinstrStack * stack);

/**
 * Push ItemInstrukce na stack
 * @param TinstrStack * stack - zasobnik
 * @param Tinstr * instr - instrukce
 */
void instr_stack_push(TinstrStack * stack, TinstrItem * instrItem);

/**
 * Stack TOP
 * @param TinstrStack * stack - zasobniku
 * @return - ItemInstrukce - vrchol zasobniku
 */
TinstrItem * instr_stack_top(TinstrStack * stack);

/**
 * Odebrani prvku z vrcholu zasobniku
 * @param TinstrStack * stack - zasobnik
 */
 void instr_stack_pop(TinstrStack * stack);

 /**
 * Smaze vsechny polozky - stack zustane jako po inicializaci
 * @param TinstrStakc * stack - zasobnik
 */
 void instr_stack_clear_all(TinstrStack * stack);

 /**
 * Smaze cely zasobnik
 * @param TinstrStack * stack - zasobnik
 */
 void instr_stack_free(TinstrStack * stack);

#endif
