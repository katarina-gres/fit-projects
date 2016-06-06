// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#ifndef _ADT_H
#define _ADT_H
#include <stdio.h>
#include "ial.h"

// struktura pro polozku v seznamu
typedef struct hashItem TListHtableItem;

struct hashItem
{
	Thtable *table;
	TListHtableItem * lptr;
	TListHtableItem * rptr;
};
// struktura obousmerne vazaneho seznamu
typedef struct 
{
	TListHtableItem *first;
	TListHtableItem * act; // act bude zaroven vzdy LAST
}TlistHtable;

typedef struct tokenItem TtokenItem;

struct tokenItem
{
	TToken * token;
	TtokenItem * next;
};

typedef struct{
	TtokenItem * first;
	TtokenItem * last;
}TlistToken;

typedef struct tokenQueueItem TtokenQueueItem;

struct tokenQueueItem{
	TToken * token;
	TtokenQueueItem * next;
};

typedef struct{
	TtokenQueueItem * first;
	TtokenQueueItem * last;
}TtokenQueue;


/*************************** Seznam pto tabulky ***************************/
/**
 * Funkce vytvori a inicializuje seznam
 * @param - 
 * @return - ukazatel na seznam / NULL pokud error 
 */
TlistHtable * htable_list_init();

/**
 * Vlozi hash tabulku na konec seznamu a posune act
 * @param - TlistHtable * myList - seznam do ktereho se ma ukladat
 * @param - Thtable * myTable - tabulka ktera se ulozi do seznamu
 * @return - ukazatel na polozku v seznamu
 */
TListHtableItem * htable_list_insert(TlistHtable *myList, Thtable *myTable);

/**
 * Smaze aktualni/posledni prvek seznamu
 * @param - TlistHtable * myList - seznam kde se bude mazat
 * @return - 
 */
void htable_list_remove_act(TlistHtable * myList);

/**
 * Zda seznam prazdny
 * @param - TlistHtable * list - seznam ktery se kontroluje
 * @return - 0 kdyz neni jina 1
 */
int htable_list_is_empty(TlistHtable * list);

/**
 * smaze vsechny polozky v seznamu - zustane jako po inicializaci
 * @param TlistHtable * list - seznam
 */
void htable_list_clear_all(TlistHtable * list);

/**
 * Uvolni vsechny tabulky ze seznamu + uvolni sebe samou
 * @param - TlistHtable * list - seznam ktery ma byt smazan
 * @return - 
 */
void htable_list_free(TlistHtable * list);

/*************************** Seznam pro tokeny ***************************/
 /**
 * Inicializace seznamu tokenu
 * @return TlistToken * - ukazatel na seznam
 */
TlistToken * token_list_init();

/**
 * Vlozeni tokenu do seznamu - uklada se nakonec
 * @param  TlistToken * list - seznam
 * @param  Ttoken * token - ukladany token
 * @return TtokenItem* - ukazatel na polozku
 */
TtokenItem * token_list_insert(TlistToken * list,TToken *token);

/**
 * Vymaze vsechny polozky, senam bude jako po inicializaci
 * @param TlistToken * list -seznam
 */
void token_list_clear_all(TlistToken *list);

/**
 * Uvolneni celeho seznamu
 * @param TlistToken * list - seznam
 */
void token_list_free(TlistToken * list);

/**
 * Inicializace fronty pro tokeny
 * @return TtokenQueue * - ukazatel na vytvorenou frontu
 */
TtokenQueue * token_queue_init();

/**
 * Zda je fronta prazdna
 * @param  queue - fronta
 * @return       1 prazna nebo neni inicializovana / 0 neprazdna
 */
int token_queue_is_empty(TtokenQueue * queue);

/**
 * vlozi token na konec fronty
 * @param TtokenQueue * queue -  fronta
 * @param TToken * token - token
 * @return   TtokenQueueItem * - ukazatel na vlozeny item
 */
TtokenQueueItem * token_queue_up(TtokenQueue * queue, TToken * token);

/**
 * pocita pocet prvku ve fronte
 * @param  TtokenQueue * queue - fronta
 * @return  int - pocet prvku
 */
int token_queue_count(TtokenQueue * queue);

/**
 * vrati ukazatel na prvni token
 * @param  TtokenQueue * queue - fronta
 * @return   TToken * - prvni token
 */
TToken * token_queue_first(TtokenQueue * queue);

/**
 * smaze prvni prvek fronty 
 * @param  TtokenQueue * queue - fronta
 * @return    TToken * - ukazatel na odstraneny token
 */
TToken * token_queue_remove(TtokenQueue * queue);

/**
 * Smaze vsechny prvky, zustane jako po inicializaci
 * @param TtokenQueue * queue - fronta
 */
void token_queuq_clear_all(TtokenQueue * queue);

/**
 * smaze celou frontu
 * @param TtokenQueue * queue - fronta
 */
void token_queue_free(TtokenQueue * queue);


#endif
