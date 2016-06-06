// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ial.h"

/**
 * Vymeni navzajom 2 prvky pola.
 * @param array pole, kde sa budu menit prvky.
 * @param left,right indexy prvkov na vymenu.
 */
void swap(char *array, int left, int right)
{
    char tmp = array[left];
    array[left] = array[right];
    array[right] = tmp;
}

/**
 * Opravi/zrekonstruuje porusenu hromadu.
 * @param array retazec/pole znakov, ktory predstavuje hromadu.
 * @param left korenovy uzol porusujuci pravidla hromady.
 * @param right velkost pola.
 */
void opravHromadu(char *array, int left, int right)
{
    int i=left;
    int j=(2*i)+1;          // index laveho syna
    char temp=array[i];
    bool podm=(j<=right); // true = lavy syn je v aktivnej(nezoradenej) casti pola
    while (podm) {
        if (j<right)     // uzol ma oba synovske uzly
            if (array[j]<array[j+1]) 
		j=j+1;	    // pravy syn je vacsi, nastav ho

        if (temp>= array[j]) { // prvok temp uz bol posunuty na svoje miesto
                podm=false; // while konci
        }
        else {
            array[i]=array[j];
            i=j;            // syn sa stane otcom pre dalsi cyklus
            j=(2*i)+1;      // nabuduci lavy syn
            podm=(j<=right);// podmienka: j nie je terminalny uzol
        }
    }
    array[i]=temp;          // konecne umiestnenie preosiateho uzlu
}

/**
 * Zoradi jednotlive znaky textoveho retazca od najnizsej ord. hodnoty.
 * @param string textovy retazec, ktoreho znaky sa budu zoradovat.
 * @param n dlzka retazca.
 */
void heapSort(char *string, int n)
{
    int left = (n/2)-1;    // index najpravsieho uzlu na najnizsej urovni
    int right = n-1;
    for (int i = left; i >= 0; i--)
    {
        opravHromadu(string,i,right);
    }

    for (right = n; right > 1; right-- )  // vlastny cyklus heapsortu
    {
        swap(string,0,right-1); // vymena korena s akt. poslednym prvkom
        opravHromadu(string,0,right-2);//znovurekonstrukcia o 1 prvok mensej hromady
    }
}

/**
 * Posle retazec na zoradenie prostrednictvom heap sortu.
 * @param string retazec, ktory sa posle na zoradenie.
 */
void sort_string(char *string)
{
    int len=strlen(string);
    heapSort(string,len);

    return;
}

/**
 * Inicializuje hashovaci tabulku podle velikosti
 * @param - int size - velikost tabulky
 * @return - ukazatel na vytvorenou tabulku
 */
Thtable * htable_init(int size){
    // alokace tabulky
    Thtable * myTable = malloc(sizeof(Thtable));
    if(myTable == NULL)
        return NULL;

    //alokace pole ukazatelu
    myTable->item = malloc(sizeof(ThtableItem)*size);
    if(myTable->item == NULL){
        free(myTable);
        return NULL;
    }
    
    //NULLovani ukazatelu    
    myTable->size = size;
    for(int i = 0;i < size;i++){
        myTable->item[i] = NULL;
    }
    return myTable;
}
/**
 * Rozptylovaci funkce
 * @param - char *str - retezec pomoci ktereho se najde ID
 * @param - int htableSize - velikost tabulky
 * @return - umisteni v tabulce
 */
unsigned int hash_function(char *str, unsigned htableSize)
{
    unsigned int h=0;
    unsigned char *p;
    for(p=(unsigned char*)str; *p!='\0'; p++)
        h = 31*h + *p;
    return h % htableSize;
}

/**
 * Vlozi token do hash tabulky
 * @param - Thtable *htable - tabulka do ktere se pridava
 * @param - TToken *token - pridavany token
 * return - ukazatel na polozku v tabulce / NULL pokud error
 */
ThtableItem * htable_insert(Thtable *htable, TToken * token){
    if(htable == NULL || token == NULL)
        return NULL;
    unsigned int position = hash_function(token->name,htable->size);

    //pokud hned prvni je prazny, tak vlozi novy
    if(htable->item[position] == NULL)
    {
        // vztvoreni tokenu a naplneni daty
        ThtableItem * myItem = malloc(sizeof(ThtableItem));
        if(myItem == NULL)
            return NULL;
        myItem->token = token;
        myItem->next = NULL;
        //ulozeni na prvni pozici
        htable->item[position] = myItem;
        return myItem;

    }
    else // pokud se nachazi nekde v seznamu
    {
        ThtableItem * item = htable->item[position];
        ThtableItem * previousItem = NULL;

        //dokud nenarazi na NULL nebo nenajde stejny token->name
        while(item != NULL)
        {
            if(strcmp(item->token->name,token->name)==0) // pokud nalezne, vrati na nej adresu
            {
                remove_token(token);
                return item;
            }
            previousItem = item;
            item = previousItem->next;
        }
        //nenalezen, vkladan nakonec
        ThtableItem * myItem = malloc(sizeof(ThtableItem));
        if(myItem == NULL)
            return NULL;
        myItem->token = token;
        myItem->next = NULL;
        previousItem->next = myItem;
        return myItem;

    }
}

/**
 *vyhleda podle token->name danou polozku a vrati ukazatel na ni
 *@param - Thtable * htable - ukazatel na tabulku
 *@param - TToken *token - hledany token
 *@return - ThtableItem* - nalezena polozka // NULL pokud error
 */
ThtableItem * htable_search(Thtable *htable,TToken * token){
    unsigned int position = hash_function(token->name,htable->size);
    ThtableItem * item = htable->item[position];
    ThtableItem * previousItem = NULL;

    //hleda dokud nenajde stejny token->name nebo nenarazi na NULL - nenalezen
    while(item != NULL)
    {
        if(strcmp(item->token->name,token->name) == 0)
            return item;
        previousItem = item;
        item = previousItem->next;
    }
    return NULL;
}
/**
 * vytiskne postupne vsechny token->name // jen pro kontrolu
 * @param - Thtable *htable - hashovaci tabulka
 * @return - void
 */
void htable_print(Thtable *htable){
    if(htable == NULL)
        return;
    for(int i=0;i<htable->size;i++){
        ThtableItem *item = htable->item[i];
        while(item != NULL){
            printf("%s\n",item->token->name);
            item = item->next;
        }
    }
}

/**
 * Vymazani jedne polozky v HASH table
 * @param - Thtable * htabel - tabulka ze ktere se bude vymazavat
 * @param - char * name - jmeno podle ktereho se vyhleda polozka
 * @return - 0 - nenalezen // 1 - uspesne
 */
void htable_remove_item(Thtable* htable, char * name){
    if (htable == NULL || name == NULL)
        return;
    unsigned int position = hash_function(name,htable->size);
    ThtableItem * item = htable->item[position];
    ThtableItem * previousItem = NULL;

    while(item != NULL)
    {
        if(strcmp(name, item->token->name) == 0) // nalezen a maze se
        {
            if(item->next == NULL && previousItem == NULL) //je na zacaku a nic za ni
                htable->item[position] = NULL;
            else if(item->next != NULL && previousItem == NULL) //je na zacatku a ma naslednicka
                htable->item[position] = item->next;
            else if(item->next == NULL && previousItem != NULL) // je na konci a ma predchudce
                previousItem->next = NULL;
            else if(item->next != NULL && previousItem != NULL) // je nekde uprostred
                previousItem->next = item->next;
            //remove_token(item->token);
            free(item);
            return;
        }
        previousItem = item;
        item = previousItem->next;
    }
    // nenalezeno
    return;
}

/**
 * uvolneni vsech polozek tabulky - zustane jako po initu
 * @param Thtable * htable - tabulka
 */
void htable_clear_all(Thtable * htable){
    if(htable == NULL)
        return;
    ThtableItem * item;
    ThtableItem * previousItem;
    for(int i = 0;i < htable->size; i++)    // prochazim vsechny radky tabulky
    {
        item = htable->item[i];
        if(item != NULL)    // pokud je radek zaplneny
        {
            while(item != NULL) // prochazim vsechny jeji polozky
            {
                previousItem = item;
                item = previousItem->next;
                //remove_token(previousItem->token);
                free(previousItem);
            }
            htable->item[i] = NULL;
        }
    }
}

/**
 * smaze celou tabulku
 * @param Thtable * htable - tabulka
 */
void htable_free(Thtable * htable){
    if(htable == NULL)
        return;
    htable_clear_all(htable); // smaze vsechny polozky
    free(htable->item); //uvolneni radku tabulky
    free(htable);   // uvoleneni cele tabulky
    htable = NULL;
    return;
}

/**
 * Odalokovani tokenu
 * @param - TToken * token - token ktery by mel byt smazan
 * @return - 
 */

void remove_token(TToken * token){
    if(token != NULL)
    {
        if(token->type == _STRINGVALUE) //pokud je to typ string
        {
            free(token->value.stringVal);   //uvolneni hodnoty
        }
        if(token->name != NULL)
            free(token->name);  //uvolenni nazvu 
        free(token);    // uvoleneni tokenu
        token = NULL;
    }
    return;
}


/**
 * Funkcia na vytvorenie pomocneho vektoru pre KMP algoritmus.
 * @param pattern podretazec, ku ktoremu sa vytvara vektor.
 * @param vektor pole, v ktorom je ulozene, s ktorym znakom patternu 
 * ma algoritmus porovnat aktualny znak retazca, ked bude porovnanie 
 * neuspesne.
 */
void vytvorVektor(char *pattern, int *vektor)
{
    int pl = strlen(pattern); 
    int i = 0;
    int j = vektor[0] = -1; // hodnota vektoru na pozicii 0 je -1
    while (i  <  pl)  //pokial nie som na konci vektoru
    {
        while (j > -1 && pattern[i] != pattern[j]) 
		j = vektor[j];
        i++;
        j++;
        if (pattern[i] == pattern[j]) 
		vektor[i] = vektor[j];
        else 
		vektor[i] = j;     
    }
}

/**
 * Knuth-Morris-Prattov algoritmus na vyhladanie prveho vyskytu 
 * zadaneho podretazca v retazci.
 * @param text retazec, v ktorom bude dany podretazec vyhladavany.
 * @param pattern vyhladavany podretazec.
 * @return vracia poziciu prveho vyskytu podretazca v retazci. 
 * V pripade nenajdenia vracia -1. Prazdny retazec sa vyskytuje 
 * v lub. retazci na pozicii 0.
 */
int find_string(char *text, char *pattern)
{
  int tl = strlen(text);
  int pl = strlen(pattern);
  int vektor[pl+1];                 
  vytvorVektor(pattern, &vektor[0]);  //vytvorenie vektora

  if (pl == 0) return 0;  //prazdny retazec

  int ppos=0;    //aktualna pozicia v patterne
  int tpos=0;    //aktualna pozicia v retazci "text"
  while (tpos < tl)  
  {
    if (pattern[ppos] == text[tpos])  //zhoda znakov (if1)
    {
      if (ppos == pl-1)  //ak som na konci patternu (if2)
      {
	return (tpos-pl+1); //uspesne vyhladavanie
      }
      else	// (else)
      {
	ppos++;
	tpos++;   //posun na dalsi znak patternu, aj retazca
      }
    }
    else if (vektor[ppos] == -1)   //nezhoda znakov 1
    {
      ppos=0;     //nastavi poziciu patternu na zaciatok
      tpos++;     //v retazci pokracuje dalej
    }
    else ppos=vektor[ppos];        //nezhoda znakov 2
  }
  return -1;  //neuspesne vyhladavanie
}



