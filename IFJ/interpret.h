// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#ifndef	INSTRUCTIONS
#define INSTRUCTIONS

#define I_END 0		// instrukce konce programu
#define I_ADD 15		// +
#define I_SUB 16		// -
#define I_MUL 17		// *
#define I_DIV 18		// /
#define I_ASSIGN 10	// prirazeni =
#define I_NOT 6
#define I_EQ 13		// ===
#define I_NEQ 14		// !==
#define I_MORE 31	// >
#define I_EMORE 12	// >=	
#define I_LESS 30	// <
#define I_ELESS 11	// <=
#define I_COND_JUMP 5	// podmineny skok pokud false => adr3 kam
#define I_JUMP 7	// instr skoku	=> adr3 kam
#define I_LABEL 1	// navesti

/*instrukce pro jednotlive zabudovane funkce*/
#define	I_BOOLVAL 2
#define I_DOUBLEVAL	3
#define I_INTVAL 4
#define I_STRVAL 19
#define I_GET_STRING 20
#define I_PUT_STRING 21
#define I_STRLEN 22
#define I_GET_SUBSTRING 23
#define I_FIND_STRING 24
#define I_SORT_STRING 25

#define I_KONKATENACE 26	//konkatenace dvou stringu
#define I_JUMPFCE 27	//skok do funkce (push next instr na zasobnik, create new table, skok do fce) adr1 - kam skocit
#define I_INSERT 28		//ve funkci vytazeni parametru a prirazeni => adr3
#define I_PUSH 29	//strci parametry do struktury => adr1 (co)
#define I_DELETE 8		//uvolni strukturu s parametry
#define I_RETURN 9	//skok z funkce, destroy table, pop next instr ze zasobniku
#define I_COND_JUMP_TRUE 32	//podmineny skok pokud true	=> adr3 kam
#define HTABLE_SIZE 32
#include "adt.h"

typedef struct instr2Item Tinstr2Item;

struct  instr2Item{
	TinstrItem * instrItem;
	char * name;
	Tinstr2Item * next;
};

typedef struct{
	Tinstr2Item * first;
	Tinstr2Item * last;
}TinstrItemList;

/*
 * interpret
 * @param TlistInstr *list - struktura ukazatelu na seznam ukazatelu na instrukci
 * @return int
 */
int interpret(TlistInstr *list);

/*
 * funkce vytvori a inicializuje strukturu Tinstr
 * @return inicializovana prazdna instrukce
 */
Tinstr *instr_init();

 /**
 * Vytvori seznam pro instructionItem
 * @return TinstrItemList * - ukazatel na seznam
 */
TinstrItemList * instrItem_list_init();

/**
 * vlozi do seznamu instrItem
 * @param  list      seznam
 * @param  instrItem polozka
 * @return           ukazatel na item v seznamu
 */
Tinstr2Item * intrItem_list_insert(TinstrItemList * list, TinstrItem * instrItem);

/**
 * vyhleda prvni shodny prvek podle jmena
 * @param  list seznam
 * @param  name hledane jmeno
 * @return      ukazatel na nalezenz instrItem
 */
TinstrItem * instrItem_list_search(TinstrItemList * list, char * name);

/**
 * smaze vsechny polozky seznamu, uvede ho do stavu po initu
 * @param list seznam
 */
void instrItem_list_clear_all(TinstrItemList * list);

/**
 * uvolni cely seznam
 * @param list seznam
 */
void instrItem_list_free(TinstrItemList * list);


#endif
