// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#ifndef _IAL_H
#define _IAL_H
#include <stdbool.h>
#include "keywords.h"
#include "instruction.h"


// unie pro ulozeni dat
typedef union{
	int intVal;
	double doubleVal;
	bool boolVal;
	char *stringVal;
}SData;
// struktura tokenu
typedef struct{
	int type;
	char *name;
	SData value;
	TinstrItem * instr;
	bool isDefine;	
}TToken;
// struktura pro polozku v hash table
typedef struct item ThtableItem;

struct item{
	TToken * token;
	ThtableItem *next;
};
// struktura hash table
typedef struct{
	int size;
	ThtableItem ** item;
}Thtable;


/**
 * Inicializuje hashovaci tabulku podle velikosti
 * @param - int size - velikost tabulky
 * @return - ukazatel na vytvorenou tabulku
 */
Thtable * htable_init(int size);

/**
 * Rozptylovaci funkce
 * @param - char *str - retezec pomoci ktereho se najde ID
 * @param - int htableSize - velikost tabulky
 * @return - umisteni v tabulce
 */
unsigned int hash_function(char *str, unsigned htableSize);

/**
 * Vlozi token do hash tabulky
 * @param - Thtable *htable - tabulka do ktere se pridava
 * @param - TToken *token - pridavany token
 * return - ukazatel na polozku v tabulce / NULL pokud error
 */
ThtableItem * htable_insert(Thtable *htable, TToken * token);

/**
 *vyhleda podle token->name danou polozku a vrati ukazatel na ni
 *@param - Thtable * htable - ukazatel na tabulku
 *@param - TToken *token - hledany token
 *@return - ThtableItem* - nalezena polozka // NULL pokud error
 */
ThtableItem * htable_search(Thtable *htable,TToken * token);

/**
 * vytiskne postupne vsechny token->name // jen pro kontrolu
 * @param - Thtable *htable - hashovaci tabulka
 * @return - void
 */
void htable_print(Thtable *htable);

/**
 * Vymazani jedne polozky v HASH table
 * @param - Thtable * htabel - tabulka ze ktere se bude vymazavat
 * @param - char * name - jmeno podle ktereho se vyhleda polozka
 * @return - 0 - nenalezen // 1 - uspesne
 */
void htable_remove_item(Thtable* htable, char * name);

/**
 * uvolneni vsech polozek tabulky - zustane jako po initu
 * @param Thtable * htable - tabulka
 */
void htable_clear_all(Thtable * htable);

/**
 * smaze celou tabulku
 * @param Thtable * htable - tabulka
 */
void htable_free(Thtable * htable);

/**
 * Odalokovani tokenu
 * @param - TToken * token - token ktery by mel byt smazan
 * @return - 
 */
void remove_token(TToken * token);


/**
 * Vymeni navzajom 2 prvky pola.
 * @param array pole, kde sa budu menit prvky.
 * @param left,right indexy prvkov na vymenu.
 */
void swap(char *array, int left, int right);

/**
 * Opravi/zrekonstruuje porusenu hromadu.
 * @param array retazec/pole znakov, ktory predstavuje hromadu.
 * @param left korenovy uzol porusujuci pravidla hromady.
 * @param right velkost pola.
 */
void opravHromadu(char *array, int left, int right);

/**
 * Zoradi jednotlive znaky textoveho retazca od najnizsej ord. hodnoty.
 * @param string textovy retazec, ktoreho znaky sa budu zoradovat.
 * @param n dlzka retazca.
 */
void heapSort(char *string, int n);

/**
 * Posle retazec na zoradenie prostrednictvom heap sortu.
 * @param string retazec, ktory sa posle na zoradenie.
 */
void sort_string(char *string);

/**
 * Funkcia na vytvorenie pomocneho vektoru pre KMP algoritmus.
 * @param pattern podretazec, ku ktoremu sa vytvara vektor.
 * @param vektor pole, v ktorom je ulozene, s ktorym znakom patternu 
 * ma algoritmus porovnat aktualny znak retazca, ked bude porovnanie 
 * neuspesne.
 */
void vytvorVektor(char *pattern, int *vektor);

/**
 * Knuth-Morris-Prattov algoritmus na vyhladanie prveho vyskytu 
 * zadaneho podretazca v retazci.
 * @param text retazec, v ktorom bude dany podretazec vyhladavany.
 * @param pattern vyhladavany podretazec.
 * @return vracia poziciu prveho vyskytu podretazca v retazci. 
 * V pripade nenajdenia vracia -1. Prazdny retazec sa vyskytuje 
 * v lub. retazci na pozicii 0.
 */
int find_string(char *text, char *pattern);

#endif
