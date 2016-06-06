// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#include "adt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Funkce vytvori a inicializuje seznam
 * @param - 
 * @return - ukazatel na seznam / NULL pokud error 
 */
TlistHtable * htable_list_init(){
    TlistHtable *myList = malloc(sizeof(TlistHtable));
    if(myList == NULL)
        return NULL;
    myList->first = NULL;
    myList->act = NULL;
    return myList;
}

/**
 * Vlozi hash tabulku na konec seznamu a posune act
 * @param - TlistHtable * myList - seznam do ktereho se ma ukladat
 * @param - Thtable * myTable - tabulka ktera se ulozi do seznamu
 * @return - ukazatel na polozku v seznamu
 */
TListHtableItem * htable_list_insert(TlistHtable *myList, Thtable *myTable){
    if(myList == NULL)
        return NULL;
    TListHtableItem * item = malloc(sizeof(TListHtableItem));
    if(item == NULL)
        return NULL;
    item->lptr = NULL;
    item->rptr = NULL;
    item->table = myTable;

    if(myList->first == NULL)   //seznam je prazdny
    {
        myList->first = item;   //oba ukazatele ukazuji na prvni polozku
        myList->act = item;
    }
    else
    {
        item->lptr = myList->act;   // posunu aktuani na vytvoreny a provazu navzajem
        myList->act->rptr = item;
        myList->act = item;
    }
    return item;
}

/**
 * Smaze aktualni/posledni prvek seznamu
 * @param - TlistHtable * myList - seznam kde se bude mazat
 * @return - 
 */
void htable_list_remove_act(TlistHtable * myList){
    if(myList->first == NULL)
        return;
    if(myList->first == myList->act) // pokud je tabulka sama v seznamu
    {
        htable_free(myList->act->table);
        free(myList->act);
        myList->act = NULL;
        myList->first = NULL;
    }
    else //pokud ma predchudce
    {
        TListHtableItem * temp = myList->act->lptr;
        temp->rptr = NULL;
        htable_free(myList->act->table);
        free(myList->act);
        myList->act = temp; // posunuti aktualniho o jeden zpatky
    }
}

/**
 * Zda seznam prazdny
 * @param - TlistHtable * list - seznam ktery se kontroluje
 * @return - 0 kdyz neni jina 1
 */
int htable_list_is_empty(TlistHtable * list){
    return(list->first == NULL ? 1 : 0);
}

/**
 * smaze vsechny polozky v seznamu - zustane jako po inicializaci
 * @param TlistHtable * list - seznam
 */
void htable_list_clear_all(TlistHtable * list){
    if(list == NULL)
        return;
    while(!htable_list_is_empty(list))
        htable_list_remove_act(list);
    return;
}

/**
 * Uvolni vsechny tabulky ze seznamu + uvolni sebe samou
 * @param - TlistHtable * list - seznam ktery ma byt smazan
 * @return - 
 */
void htable_list_free(TlistHtable * list){
    if(list == NULL)
        return;
    htable_list_clear_all(list);
    free(list);
    list = NULL;
}


/**
 * Inicializace seznamu tokenu
 * @return TlistToken * - ukazatel na seznam
 */
TlistToken * token_list_init(){
    TlistToken * list = malloc(sizeof(TlistToken));
    if(list == NULL)
        return NULL;
    list->first = NULL;
    list->last = NULL;
    return list;
}

/**
 * Vlozeni tokenu do seznamu - uklada se nakonec
 * @param  TlistToken * list - seznam
 * @param  Ttoken * token - ukladany token
 * @return TtokenItem* - ukazatel na polozku
 */
TtokenItem * token_list_insert(TlistToken * list,TToken *token){
    if(list == NULL)
       return NULL;
    TtokenItem * item = malloc(sizeof(TtokenItem));
    if(item == NULL)
        return NULL;
    item->next = NULL;
    item->token = token;
    if(list->first == NULL)
    {
        list->first = item;
        list->last = item;   
    }
    else
    {
        list->last->next = item;
        list->last = item;
    }
    return item;
}
/**
 * Vymaze vsechny polozky, senam bude jako po inicializaci
 * @param TlistToken * list -seznam
 */
void token_list_clear_all(TlistToken *list){
    if(list == NULL)
        return;
    TtokenItem * temp = list->first;
    while(temp != NULL)
    {
        list->first = temp->next;
        remove_token(temp->token);
        free(temp);
        temp = list->first;
    }
    list->first = NULL;
    list->last = NULL;
}

/**
 * Uvolneni celeho seznamu
 * @param TlistToken * list - seznam
 */
void token_list_free(TlistToken * list){
    if(list == NULL)
        return;
    token_list_clear_all(list);
    free(list);
    list = NULL;
    return;
}

/**
 * Inicializace fronty pro tokeny
 * @return TtokenQueue * - ukazatel na vytvorenou frontu
 */
TtokenQueue * token_queue_init(){
    TtokenQueue * queue = malloc(sizeof(TtokenQueue));
    if(queue == NULL)
        return NULL;
    queue->first = NULL;
    queue->last = NULL;
    return queue;
}

/**
 * Zda je fronta prazdna
 * @param  queue - fronta
 * @return       1 prazna nebo neni inicializovana / 0 neprazdna
 */
int token_queue_is_empty(TtokenQueue * queue){
    if(queue == NULL)
        return 1;
    return(queue->first == NULL ? 1 : 0);
}

/**
 * vlozi token na konec fronty
 * @param TtokenQueue * queue -  fronta
 * @param TToken * token - token
 * @return   TtokenQueueItem * - ukazatel na vlozeny item
 */
TtokenQueueItem * token_queue_up(TtokenQueue * queue, TToken * token){
    if(queue == NULL || token == NULL)
        return NULL;
    TtokenQueueItem * item = malloc(sizeof(TtokenQueueItem));
    if(item == NULL)
        return NULL;
    item->token = token;
    item->next = NULL;
    if(token_queue_is_empty(queue)) // vkldani do prazdne
    {
        queue->first = item;
        queue->last = item;
        return item;
    }
    else
    {
        queue->last->next = item;
        queue->last = item;
        return item;
    }
}

/**
 * pocita pocet prvku ve fronte
 * @param  TtokenQueue * queue - fronta
 * @return  int - pocet prvku
 */
int token_queue_count(TtokenQueue * queue){
    int count = 0;
    TtokenQueueItem *temp = queue->first;
    while(temp != NULL)
    {
        count++;
        temp = temp->next;
    }
    return count;
}

/**
 * vrati ukazatel na prvni token
 * @param  TtokenQueue * queue - fronta
 * @return   TToken * - prvni token
 */
TToken * token_queue_first(TtokenQueue * queue){
    if(queue == NULL)
        return NULL;
    return queue->first->token;
}

/**
 * smaze prvni prvek fronty 
 * @param  TtokenQueue * queue - fronta
 * @return    TToken * - ukazatel na odstraneny token
 */
TToken * token_queue_remove(TtokenQueue * queue){
    if(queue == NULL)
        return NULL;
    if(queue->first == NULL)
        return NULL;
    TToken * token = queue->first->token;
    if(queue->first == queue->last) // maze jediny
    {
        free(queue->first);
        queue->first = NULL;
        queue->last = NULL;
    }
    else
    {
        TtokenQueueItem * temp = queue->first->next;
        free(queue->first);
        queue->first = temp;
    }
    return token;
}

/**
 * Smaze vsechny prvky, zustane jako po inicializaci
 * @param TtokenQueue * queue - fronta
 */
void token_queuq_clear_all(TtokenQueue * queue){
    if(queue == NULL)
        return;
    TtokenQueueItem *temp = NULL;
    while(queue->first != NULL)
    {
        temp = queue->first->next;
        free(queue->first);
        queue->first = temp;
    }
    queue->first = NULL;
    queue->last = NULL;
    return;
}

/**
 * smaze celou frontu
 * @param TtokenQueue * queue - fronta
 */
void token_queue_free(TtokenQueue * queue){
    token_queuq_clear_all(queue); // maze vsechny polozky
    free(queue);
    queue = NULL;
}

/*******************************************/
/*void main(){
    TToken * token = malloc(sizeof(TToken));
    if(token == NULL)
        return;
    TToken * token2 = malloc(sizeof(TToken));
    if(token == NULL)
        return;
    TToken * token3 = malloc(sizeof(TToken));
    if(token == NULL)
        return;
    token->name = malloc(10);
    token2->name = malloc(10);
    token3->name = malloc(10);
    strcpy(token->name, "prvni");
    printf("%s\n", token->name);
    strcpy(token2->name, "druhz");
    printf("%s\n", token2->name);
    strcpy(token3->name, "treti");
    printf("%s\n", token3->name);
    token->value.intVal = 1;
    token2->value.intVal = 2;
    token3->value.intVal = 3;
    token->type = _INTVALUE;
    token2->type = _INTVALUE;
    token3->type = _INTVALUE;
    TtokenQueue * queue = token_queue_init();
    printf("pocet: %d\n",token_queue_count(queue));
    if(token_queue_is_empty(queue))
        printf("je praznda\n");
    else
        printf("neni prazdna\n");
    printf("vkladam 1 token\n");
    token_queue_up(queue, token);
    printf("pocet: %d\n",token_queue_count(queue));
    if(token_queue_is_empty(queue))
        printf("je praznda\n");
    else
        printf("neni prazdna\n");

    printf("vkladam 2 token\n");
    token_queue_up(queue, token2);
    printf("pocet: %d\n",token_queue_count(queue));
    if(token_queue_is_empty(queue))
        printf("je praznda\n");
    else
        printf("neni prazdna\n");

    printf("vkladam 3 token\n");
    token_queue_up(queue, token3);
    printf("pocet: %d\n",token_queue_count(queue));
    if(token_queue_is_empty(queue))
        printf("je praznda\n");
    else
        printf("neni prazdna\n");
    TToken * mujToken = token_queue_first(queue);
    printf("%s\n",mujToken->name);
    printf("pocet: %d\n",token_queue_count(queue));
    mujToken = token_queue_remove(queue);
    printf("%s\n",mujToken->name);
    printf("pocet: %d\n",token_queue_count(queue));
    mujToken = token_queue_remove(queue);
    printf("%s\n",mujToken->name);
    printf("pocet: %d\n",token_queue_count(queue));
    mujToken = token_queue_first(queue);
    printf("%s\n",mujToken->name);
    mujToken = token_queue_remove(queue);
    printf("%s\n",mujToken->name);
    printf("pocet: %d\n",token_queue_count(queue));

    //token_queuq_clear_all(queue);
    token_queue_free(queue);
    
}*/
