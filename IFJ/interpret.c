// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "interpret.h"
#include "built_in.h"
#include "keywords.h"
#include "adt.h"
#include "ial.h"
#include <string.h>
#include "instruction.h"
#include "global.h"

Tinstr *instr_init(){
	Tinstr *instruction = NULL;
	if((instruction = malloc(sizeof(Tinstr))) == NULL) return NULL;

	instruction->adr1 = NULL;
	instruction->adr2 = NULL;
	instruction->adr3 = NULL;
	instruction->instr_type = -1;

	return instruction;
}

/**
 * Vytvori seznam pro instructionItem
 * @return TinstrItemList * - ukazatel na seznam
 */
TinstrItemList * instrItem_list_init(){
	TinstrItemList * list = malloc(sizeof(TinstrItemList));
	if(list == NULL)
		return NULL;
	list->first = NULL;
	list->last = NULL;
	return list;
}

/**
 * vlozi do seznamu instrItem
 * @param  list      seznam
 * @param  instrItem polozka
 * @return           ukazatel na item v seznamu
 */
Tinstr2Item * intrItem_list_insert(TinstrItemList * list, TinstrItem * instrItem){
	if(list == NULL || instrItem == NULL)
		return NULL;
	Tinstr2Item * item = malloc(sizeof(Tinstr2Item));
	if(item == NULL)
		return NULL;
	item->instrItem = instrItem;
	item->name = ((TToken *)(instrItem->instruction->adr1))->value.stringVal; // ulozeni nazvu
	item->next = NULL;
	if(list->first == NULL) // seznam je prazdny
	{
		list->first = item;
		list->last = item;
		return item;
	}
	else
	{	
		Tinstr2Item * temp = list->last;
		temp->next = item;
		list->last = item;
		return item;
	}	
}

/**
 * vyhleda prvni shodny prvek podle jmena
 * @param  list seznam
 * @param  name hledane jmeno
 * @return      ukazatel na nalezenz instrItem
 */
TinstrItem * instrItem_list_search(TinstrItemList * list, char * name){
	if(list == NULL || name == NULL)
		return NULL;
	Tinstr2Item * temp = list->first;
	while(temp != NULL)
	{
		if(strcmp(temp->name,name) == 0)
			return temp->instrItem;
		temp = temp->next;
	}
	return NULL; // nenalezen
}

/**
 * smaze vsechny polozky seznamu, uvede ho do stavu po initu
 * @param list seznam
 */
void instrItem_list_clear_all(TinstrItemList * list){
	if(list == NULL)
		return;
	Tinstr2Item * temp = list->first;
	while(list->first != NULL)
	{
		temp = list->first->next;
		free(list->first);
		list->first = temp;
	}
	list->first = NULL;
	list->last = NULL;
	return;
}

/**
 * uvolni cely seznam
 * @param list seznam
 */
void instrItem_list_free(TinstrItemList * list){
	if(list == NULL)
		return;
	instrItem_list_clear_all(list);
	free(list);
	list = NULL;
	return;
}

#define destroy_all() do{\
	token_queue_free(token_que);\
	instr_stack_free(instruction_stack);\
	instrItem_list_free(label_list);\
	if(new_token != NULL){token_list_insert(g_token_list, new_token);}\
	if(g_return.type == _STRINGVALUE){free(g_return.value.stringVal);}\
	}while(0)

/* to co je prozatim zakomentovane je pouze ilustracni,
 * ne osetrene, ne korektni, ne konecna implementace
 */

int interpret(TlistInstr *list){
	bool compare_flag = true;
	int errno = 0;
	int count_params = 0;
	char *temp_string_destroy = NULL;
	char *temp_string_destroy2 = NULL;
//	char *temp_string = NULL; //pomocna
	Tinstr *instr = NULL;	//pomocna, cte se z ni instrukce
	TToken *adr1, *adr2, *adr3 = NULL;
	TToken *new_token = NULL;	//pomocna pro novy token
	TToken *insert_token = NULL;	//pomocna pro token I_INSERT
	TinstrStack *instruction_stack = instruction_stack = instr_stack_init();	//stack ukazatelu na instritem
	TtokenQueue *token_que = token_queue_init();	//fronta tokenu (pro parametry)
	TinstrItemList *label_list = instrItem_list_init();	//seznam labelu
	ThtableItem *hash_item = NULL;
	/*nacteni vsech labelu do seznamu*/
	instr_list_first(list);
	while(list->act != list->last){
		if(list->act->instruction->instr_type == I_LABEL){
			if(intrItem_list_insert(label_list, list->act) == NULL){
				destroy_all();
				return 99; //asi 99, nepodarilo se vlozit label do seznamu
			}
		}
		instr_list_next(list);
	}
	instr_list_first(list);

	while(1){
		instr = instr_list_get_data(list);
		adr1 = NULL;
		adr2 = NULL;
		adr3 = NULL;
		new_token = NULL;
		if(instr == NULL){
			destroy_all();
			return 99;
		}

		if(instr->adr1 != NULL){
			if(((TToken *)(instr->adr1))->name != NULL){
				if((hash_item = (htable_search((g_htable->act->table),
				((TToken *)(instr->adr1))))) == NULL){
					adr1 = NULL;
				}
				else{
					adr1 = hash_item->token;
				}
			}
			else{
				adr1 = (TToken *)(instr->adr1);
			}
		}

		if(instr->adr2 != NULL){
			if(((TToken *)(instr->adr2))->name != NULL){
				if((hash_item = (htable_search((g_htable->act->table),
				((TToken *)(instr->adr2))))) == NULL){
					adr2 = NULL;
				}
				else{
					adr2 = hash_item->token;
				}
			}
			else{
				adr2 = (TToken *)(instr->adr2);
			}
		}

		if(instr->adr3 != NULL){
			if(((TToken *)(instr->adr3))->name != NULL){
				if((hash_item = (htable_search((g_htable->act->table),
				((TToken *)(instr->adr3))))) == NULL){
					adr3 = NULL;
				}
				else{
					adr3 = hash_item->token;
				}
			}
			else{
				adr3 = (TToken *)(instr->adr3);
			}
		}

		switch(instr->instr_type){
			case I_END:
				destroy_all();
				return 0;
				break;

		/*************** aritmeticke operace a konkatenace *****************/

			case I_ADD:
				if(instr->adr1 != NULL && instr->adr2 != NULL && instr->adr3 != NULL){	
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 5; //asi 12
					}


					if(adr3 == NULL){
						if((new_token = malloc(sizeof(TToken))) == NULL){
							destroy_all();
							return 99;
						}
						new_token->type = _NULLVALUE;
						if((new_token->name = malloc(
						strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
							destroy_all();
							return 99;
						}
						strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
						if(htable_insert(g_htable->act->table, new_token) == NULL){
							destroy_all();
							return 99;
						}
						adr3 = new_token;

					}

					if(adr1->type == _INTVALUE && adr2->type == _INTVALUE){
						adr3->value.intVal = 
						adr1->value.intVal + adr2->value.intVal;
						adr3->type = _INTVALUE;
					}
					else if((adr1->type == _DOUBLEVALUE || adr1->type == _INTVALUE)
					&& (adr2->type == _DOUBLEVALUE || adr2->type == _INTVALUE)){

						adr3->value.doubleVal =
						double_val(adr1, &errno) + double_val(adr2, &errno);
						adr3->type = _DOUBLEVALUE;
					}
					else{
						destroy_all();
						return 12;	//error, nespravne typy
					}
				}
				else{
					destroy_all();
					return 99; //asi 12
				}
				break;

			case I_SUB:
				if(instr->adr1 != NULL && instr->adr2 != NULL && instr->adr3 != NULL){	
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 5; //asi 12
					}

					if(adr3 == NULL){
						if((new_token = malloc(sizeof(TToken))) == NULL){
							destroy_all();
							return 99;
						}
						new_token->type = _NULLVALUE;
						if((new_token->name = malloc(
						strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
							destroy_all();
							return 99;
						}
						strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
						if(htable_insert(g_htable->act->table, new_token) == NULL){
							destroy_all();
							return 99;
						}
						adr3 = new_token;
					}

					if(adr1->type == _INTVALUE && adr2->type == _INTVALUE){
						adr3->value.intVal = 
						adr1->value.intVal - adr2->value.intVal;
						adr3->type = _INTVALUE;
					}
					else if((adr1->type == _DOUBLEVALUE || adr1->type == _INTVALUE)
					&& (adr2->type == _DOUBLEVALUE || adr2->type == _INTVALUE)){

						adr3->value.doubleVal =
						double_val(adr1, &errno) - double_val(adr2, &errno);
						adr3->type = _DOUBLEVALUE;
					}
					else{
						destroy_all();
						return 12;	//error, nespravne typy
					}
				}
				else{
					destroy_all();
					return 99; //asi 12
				}
				break;

			case I_MUL:
				if(instr->adr1 != NULL && instr->adr2 != NULL && instr->adr3 != NULL){	
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 5; //asi 12
					}

					if(adr3 == NULL){
						if((new_token = malloc(sizeof(TToken))) == NULL){
							destroy_all();
							return 99;
						}
						new_token->type = _NULLVALUE;
						if((new_token->name = malloc(
						strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
							destroy_all();
							return 99;
						}
						strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
						if(htable_insert(g_htable->act->table, new_token) == NULL){
							destroy_all();
							return 99;
						}
						adr3 = new_token;
					}

					if(adr1->type == _INTVALUE && adr2->type == _INTVALUE){
						adr3->value.intVal = 
						adr1->value.intVal * adr2->value.intVal;
						adr3->type = _INTVALUE;
					}
					else if((adr1->type == _DOUBLEVALUE || adr1->type == _INTVALUE)
					&& (adr2->type == _DOUBLEVALUE || adr2->type == _INTVALUE)){

						adr3->value.doubleVal =
						double_val(adr1, &errno) * double_val(adr2, &errno);
						adr3->type = _DOUBLEVALUE;
					}
					else{
						destroy_all();
						return 12;	//error, nespravne typy
					}
				}
				else{
					destroy_all();
					return 99; //asi 12
				}
				break;

			case I_DIV:
				if(instr->adr1 != NULL && instr->adr2 != NULL && instr->adr3 != NULL){		
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 5; //asi 12
					}

					if(adr3 == NULL){
						if((new_token = malloc(sizeof(TToken))) == NULL){
							destroy_all();
							return 99;
						}
						new_token->type = _NULLVALUE;
						if((new_token->name = malloc(
						strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
							destroy_all();
							return 99;
						}
						strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
						if(htable_insert(g_htable->act->table, new_token) == NULL){
							destroy_all();
							return 99;
						}
						adr3 = new_token;
					}

					if((adr1->type == _DOUBLEVALUE || adr1->type == _INTVALUE)
					&& (adr2->type == _DOUBLEVALUE || adr2->type == _INTVALUE)){
						if(adr2->type == _DOUBLEVALUE){
							if(adr2->value.doubleVal == 0.0){
								destroy_all();
								return 10;	// deleni nulou
							}
						}
						else{
							if(adr2->value.intVal == 0){
								destroy_all();
								return 10; // deleni nulou
							}
						}
						adr3->value.doubleVal =
						double_val(adr1, &errno) / double_val(adr2, &errno);
						adr3->type = _DOUBLEVALUE;
					}
					else{
						destroy_all();
						return 12;	//error, nespravne typy
					}
				}
				else{
					destroy_all();
					return 99; //asi 12
				}
				break;

			case I_ASSIGN:
				if(instr->adr1 != NULL && instr->adr3 != NULL){
					if(adr1 == NULL){
						destroy_all();
						return 5; //asi 12
					}


					if(adr3 == NULL){
						if((new_token = malloc(sizeof(TToken))) == NULL){
							destroy_all();
							return 99;
						}
						new_token->type = _NULLVALUE;
						if((new_token->name = malloc(
						strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
							destroy_all();
							return 99;
						}
						strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
						if(htable_insert(g_htable->act->table, new_token) == NULL){
							destroy_all();
							return 99;
						}
						adr3 = new_token;
					}
					if(adr1->type == _INTVALUE){
						if(adr3->type == _STRINGVALUE){
							free(adr3->value.stringVal);
						}
						adr3->type = _INTVALUE;
						adr3->value.intVal = adr1->value.intVal;
					}
					else if(adr1->type == _DOUBLEVALUE){
						if(adr3->type == _STRINGVALUE){
							free(adr3->value.stringVal);
						}
						adr3->type = _DOUBLEVALUE;
						adr3->value.doubleVal =	adr1->value.doubleVal;
					}
					else if(adr1->type == _BOOLVALUE){
						if(adr3->type == _STRINGVALUE){
							free(adr3->value.stringVal);
						}
						adr3->type = _BOOLVALUE;
						adr3->value.boolVal = adr1->value.boolVal;
					}else if(adr1->type == _NULLVALUE){
						if(adr3->type == _STRINGVALUE){
							free(adr3->value.stringVal);
						}
						adr3->type = _NULLVALUE;
					}
					else if(adr1->type == _STRINGVALUE){
						if(adr3->type == _STRINGVALUE){
							free(adr3->value.stringVal);
						}
						adr3->type = _STRINGVALUE;
						if((adr3->value.stringVal = malloc(strlen(adr1->value.stringVal) + 1)) == NULL){
							destroy_all();
							return 99;
						}
						strcpy(adr3->value.stringVal, adr1->value.stringVal);
					}
					else{
						destroy_all();
						return 99; //nekorektni stav
					}
				}
				if(instr->adr1 != NULL){
					if(adr1 == NULL){
						destroy_all();
						return 5; //asi 12
					}
			
					switch(adr1->type){
						case _INTVALUE:
							if(adr1->value.intVal == 0)
								compare_flag = false;
							else
								compare_flag = true;
							break;

						case _DOUBLEVALUE:
							if(adr1->value.doubleVal == 0.0)
								compare_flag = false;
							else
								compare_flag = true;
							break;

						case _BOOLVALUE:
							if(adr1->value.boolVal == false)
								compare_flag = false;
							else
								compare_flag = true;
							break;

						case _STRINGVALUE:
							if(strcmp("",adr1->value.stringVal) <= 0)
								compare_flag = false;
							else
								compare_flag = true;
							break;

						case _NULLVALUE:
							compare_flag = false;
							break;

						default:
							break;
					}
				}
				break;

			case I_KONKATENACE:
				if(instr->adr1 != NULL && instr->adr2 != NULL && instr->adr3 != NULL){
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 5; //asi 12
					}

					if(adr1->type != _STRINGVALUE){
						destroy_all();
						return 12; //nespravnost typu, prvni neni string
					}

					if(adr3 == NULL){
						if((new_token = malloc(sizeof(TToken))) == NULL){
							destroy_all();
							return 99;
						}
						new_token->type = _NULLVALUE;
						if((new_token->name = malloc(
						strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
							destroy_all();
							return 99;
						}
						strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
						if(htable_insert(g_htable->act->table, new_token) == NULL){
							destroy_all();
							return 99;
						}
						adr3 = new_token;
					}

					if(adr3->type == _STRINGVALUE){
						free(adr3->value.stringVal);
					}

					if((temp_string_destroy = str_val(adr2, &errno)) != NULL && 
					(temp_string_destroy2 = str_val(adr1, &errno)) != NULL){
						adr3->value.stringVal = konkatenace(temp_string_destroy2, temp_string_destroy);
						adr3->type = _STRINGVALUE;
						free(temp_string_destroy2);
						free(temp_string_destroy);
					}
					else{
						destroy_all();
						return 99;
					}
				}
				break;

		/******************porovnavani a logicke operace*********************/

			case I_NOT:
				if(instr->adr1 != NULL){		//pokud je adresa, negace priznaku, negace hodnoty na adrese
					if(adr1 == NULL){
						if((new_token = malloc(sizeof(TToken))) == NULL){
							destroy_all();
							return 99;
						}
						new_token->type = _NULLVALUE;
						if((new_token->name = malloc(
						strlen(((TToken *)(instr->adr1))->name) + 1)) == NULL){
							destroy_all();
							return 99;
						}
						strcpy(new_token->name, ((TToken *)(instr->adr1))->name);
						if(htable_insert(g_htable->act->table, new_token) == NULL){
							destroy_all();
							return 99;
						}
						adr1 = new_token;
					}

					if(adr1->value.boolVal == true){
						adr1->value.boolVal = false;
						compare_flag = false;
					}
					else{
						adr1->value.boolVal = true;
						compare_flag = true;
					}
				}
				else{		//pokud neni adresa, zmena pouze priznaku
					if(compare_flag == true)
						compare_flag = false;
					else
						compare_flag = true;
				}
				break;

			case I_EQ:
				if(instr->adr1 != NULL && instr->adr2 != NULL){
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 5; //asi 12
					}
					
					compare_flag = false;
					if(adr1->type == _INTVALUE && adr2->type == _INTVALUE){
	
						if(adr1->value.intVal == adr2->value.intVal){		//porovnani
							compare_flag = true;		//nastavime priznak pro jump
						}
						else
							compare_flag = false;
					}
					else if(adr1->type == _DOUBLEVALUE && adr2->type == _DOUBLEVALUE){

						if(adr1->value.doubleVal == adr2->value.doubleVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _BOOLVALUE && adr2->type == _BOOLVALUE){

						if(adr1->value.boolVal == adr2->value.boolVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _STRINGVALUE && adr2->type == _STRINGVALUE){
						
						if(!strcmp(adr1->value.stringVal, adr2->value.stringVal))
							compare_flag = true;
						else 
							compare_flag = false;
					}
					else if(adr1->type == _NULLVALUE && adr2->type == _NULLVALUE)
						compare_flag = true;

					if(instr->adr3 != NULL){		//pokud je uvedena i vysledna adresa, ulozime zde vysledek porovani
						if(adr3 == NULL){
							if((new_token = malloc(sizeof(TToken))) == NULL){
								destroy_all();
								return 99;
							}
							new_token->type = _NULLVALUE;
							if((new_token->name = malloc(
							strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
								destroy_all();
								return 99;
							}
							strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
							if(htable_insert(g_htable->act->table, new_token) == NULL){
								destroy_all();
								return 99;
							}
							adr3 = new_token;
						}
						adr3->value.boolVal = compare_flag;
						adr3->type = _BOOLVALUE;
					}
				}
				break;

			case I_NEQ:
				if(instr->adr1 != NULL && instr->adr2 != NULL){
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 5; //asi 12
					}
					compare_flag = true;
					if(adr1->type == _INTVALUE && adr2->type == _INTVALUE){

						if(adr1->value.intVal != adr2->value.intVal){		//porovnani
							compare_flag = true;		//nastavime priznak pro jump
						}
						else
							compare_flag = false;
					}
					else if(adr1->type == _DOUBLEVALUE && adr2->type == _DOUBLEVALUE){

						if(adr1->value.doubleVal != adr2->value.doubleVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _BOOLVALUE && adr2->type == _BOOLVALUE){

						if(adr1->value.boolVal != adr2->value.boolVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _STRINGVALUE && adr2->type == _STRINGVALUE){
						
						if(strcmp(adr1->value.stringVal, adr2->value.stringVal))
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _NULLVALUE && adr2->type == _NULLVALUE)
						compare_flag = false;

					if(instr->adr3 != NULL){

						if(adr3 == NULL){
							if((new_token = malloc(sizeof(TToken))) == NULL){
								destroy_all();
								return 99;
							}
							new_token->type = _NULLVALUE;
							if((new_token->name = malloc(
							strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
								destroy_all();
								return 99;
							}
							strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
							if(htable_insert(g_htable->act->table, new_token) == NULL){
								destroy_all();
								return 99;
							}
							adr3 = new_token;
						}
						adr3->value.boolVal = compare_flag;
						adr3->type = _BOOLVALUE;
					}
				}
				break;
			//
			//
			//
			//chybi porovnani stringu jeste
			//
			//
			//
			case I_MORE:
				if(instr->adr1 != NULL && instr->adr2 != NULL){
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 5; //asi 12
					}

					if(adr1->type == _INTVALUE && adr2->type == _INTVALUE){

						if(adr1->value.intVal > adr2->value.intVal){		//porovnani
							compare_flag = true;		//nastavime priznak pro jump
						}
						else
							compare_flag = false;
					}
					else if(adr1->type == _DOUBLEVALUE && adr2->type == _DOUBLEVALUE){

						if(adr1->value.doubleVal > adr2->value.doubleVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _BOOLVALUE && adr2->type == _BOOLVALUE){

						if(adr1->value.boolVal > adr2->value.boolVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _NULLVALUE && adr2->type == _NULLVALUE){
						compare_flag = false;
					}
					else if(adr1->type == _STRINGVALUE && adr2->type == _STRINGVALUE){
						if(strcmp(adr1->value.stringVal, adr2->value.stringVal) > 0){
							compare_flag = true;
						}
						else{
							compare_flag = false;
						}
					}
					else{
						destroy_all();
						return 12;
					}

					if(instr->adr3 != NULL){
						if(adr3 == NULL){
							if((new_token = malloc(sizeof(TToken))) == NULL){
								destroy_all();
								return 99;
							}
							new_token->type = _NULLVALUE;
							if((new_token->name = malloc(
							strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
								destroy_all();
								return 99;
							}
							strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
							if(htable_insert(g_htable->act->table, new_token) == NULL){
								destroy_all();
								return 99;
							}
							adr3 = new_token;
						}
						adr3->value.boolVal = compare_flag;
						adr3->type = _BOOLVALUE;
					}
				}
				break;

			case I_EMORE:
				if(instr->adr1 != NULL && instr->adr2 != NULL){
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 5; //asi 12
					}

					if(adr1->type == _INTVALUE && adr2->type == _INTVALUE){

						if(adr1->value.intVal >= adr2->value.intVal){		//porovnani
							compare_flag = true;		//nastavime priznak pro jump
						}
						else
							compare_flag = false;
					}
					else if(adr1->type == _DOUBLEVALUE && adr2->type == _DOUBLEVALUE){

						if(adr1->value.doubleVal >= adr2->value.doubleVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _BOOLVALUE && adr2->type == _BOOLVALUE){

						if(adr1->value.boolVal >= adr2->value.boolVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _NULLVALUE && adr2->type == _NULLVALUE){
						compare_flag = true;
					}
					else if(adr1->type == _STRINGVALUE && adr2->type == _STRINGVALUE){
						if(strcmp(adr1->value.stringVal, adr2->value.stringVal) >= 0){
							compare_flag = true;
						}
						else{
							compare_flag = false;
						}
					}
					else{
						destroy_all();
						return 12;
					}

					if(instr->adr3 != NULL){
						if(adr3 == NULL){
							if((new_token = malloc(sizeof(TToken))) == NULL){
								destroy_all();
								return 99;
							}
							new_token->type = _NULLVALUE;
							if((new_token->name = malloc(
							strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
								destroy_all();
								return 99;
							}
							strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
							if(htable_insert(g_htable->act->table, new_token) == NULL){
								destroy_all();
								return 99;
							}
							adr3 = new_token;
						}

						adr3->value.boolVal = compare_flag;
						adr3->type = _BOOLVALUE;
					}
				}
				break;

			case I_LESS:
				if(instr->adr1 != NULL && instr->adr2 != NULL){
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 5; //asi 12
					}

					if(adr1->type == _INTVALUE && adr2->type == _INTVALUE){

						if(adr1->value.intVal < adr2->value.intVal){		//porovnani
							compare_flag = true;		//nastavime priznak pro jump
						}
						else
							compare_flag = false;
					}
					else if(adr1->type == _DOUBLEVALUE && adr2->type == _DOUBLEVALUE){

						if(adr1->value.doubleVal < adr2->value.doubleVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _BOOLVALUE && adr2->type == _BOOLVALUE){

						if(adr1->value.boolVal < adr2->value.boolVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _NULLVALUE && adr2->type == _NULLVALUE){
						compare_flag = false;
					}
					else if(adr1->type == _STRINGVALUE && adr2->type == _STRINGVALUE){
						if(strcmp(adr1->value.stringVal, adr2->value.stringVal) < 0){
							compare_flag = true;
						}
						else{
							compare_flag = false;
						}
					}
					else{
						destroy_all();
						return 12;
					}

					if(instr->adr3 != NULL){
						if(adr3 == NULL){
							if((new_token = malloc(sizeof(TToken))) == NULL){
								destroy_all();
								return 99;
							}
							new_token->type = _NULLVALUE;
							if((new_token->name = malloc(
							strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
								destroy_all();
								return 99;
							}
							strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
							if(htable_insert(g_htable->act->table, new_token) == NULL){
								destroy_all();
								return 99;
							}
							adr3 = new_token;
						}
						adr3->value.boolVal = compare_flag;
						adr3->type = _BOOLVALUE;
					}
				}
				break;

			case I_ELESS:
				if(instr->adr1 != NULL && instr->adr2 != NULL){
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 5; //asi 12
					}

					if(adr1->type == _INTVALUE && adr2->type == _INTVALUE){

						if(adr1->value.intVal <= adr2->value.intVal){		//porovnani
							compare_flag = true;		//nastavime priznak pro jump
						}
						else
							compare_flag = false;
					}
					else if(adr1->type == _DOUBLEVALUE && adr2->type == _DOUBLEVALUE){

						if(adr1->value.doubleVal <= adr2->value.doubleVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _BOOLVALUE && adr2->type == _BOOLVALUE){

						if(adr1->value.boolVal <= adr2->value.boolVal)
							compare_flag = true;
						else
							compare_flag = false;
					}
					else if(adr1->type == _NULLVALUE && adr2->type == _NULLVALUE){
						compare_flag = true;
					}
					else if(adr1->type == _STRINGVALUE && adr2->type == _STRINGVALUE){
						if(strcmp(adr1->value.stringVal, adr2->value.stringVal) <= 0){
							compare_flag = true;
						}
						else{
							compare_flag = false;
						}
					}
					else{
						destroy_all();
						return 12; //asi 12
					}

					if(instr->adr3 != NULL){
						if(adr3 == NULL){
							if((new_token = malloc(sizeof(TToken))) == NULL){
								destroy_all();
								return 99;
							}
							new_token->type = _NULLVALUE;
							if((new_token->name = malloc(
							strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
								destroy_all();
								return 99;
							}
							strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
							if(htable_insert(g_htable->act->table, new_token) == NULL){
								destroy_all();
								return 99;
							}
							adr3 = new_token;
						}
						adr3->value.boolVal = compare_flag;
						adr3->type = _BOOLVALUE;
					}
				}
				break;

	/*********************** skoky a prace s fcemi **************************/

			case I_COND_JUMP:
				if(instr->adr3 != NULL)
					if(!compare_flag){
						instr_list_goto(list, 
						instrItem_list_search(label_list, ((TToken *)(instr->adr3))->value.stringVal));
					}
				break;

			case I_COND_JUMP_TRUE:
				if(instr->adr3 != NULL)
					if(compare_flag){
						instr_list_goto(list,
						instrItem_list_search(label_list, ((TToken *)(instr->adr3))->value.stringVal));
					}
				break;

			case I_JUMP:
				if(instr->adr3 != NULL){
					instr_list_goto(list,
					instrItem_list_search(label_list, ((TToken *)(instr->adr3))->value.stringVal));
				}
				break;

			case I_JUMPFCE:
				if(instr->adr3 != NULL){
					instr_stack_push(instruction_stack, intr_list_get_next(list));
					if(htable_list_insert(g_htable, htable_init(HTABLE_SIZE)) == NULL){
						destroy_all();
						return 99; // nepodarilo se vytvorit tabulku pro fci
					}
					instr_list_goto(list,
					instrItem_list_search(label_list, ((TToken *)(instr->adr3))->value.stringVal));
				}
				break;

			case I_INSERT:
				if(token_queue_count(token_que)){
					insert_token = token_queue_remove(token_que);
					if((new_token = malloc(sizeof(TToken))) == NULL){
						destroy_all();
						return 99;
					}
					new_token->type = _NULLVALUE;
					if((new_token->name = malloc(strlen(((TToken *)(instr->adr3))->name) + 1)) == NULL){
						destroy_all();
						return 99;
					}
					strcpy(new_token->name, ((TToken *)(instr->adr3))->name);
					switch(insert_token->type){
						case _INTVALUE:
							new_token->value.intVal = insert_token->value.intVal;
							new_token->type = _INTVALUE;
							break;

						case _DOUBLEVALUE:
							new_token->value.doubleVal = insert_token->value.doubleVal;
							new_token->type = _DOUBLEVALUE;
							break;

						case _BOOLVALUE:
							new_token->value.boolVal = insert_token->value.boolVal;
							new_token->type = _BOOLVALUE;
							break;

						case _STRINGVALUE:				
							if((new_token->value.stringVal = malloc(strlen(insert_token->value.stringVal) + 1)) == NULL){
								destroy_all();
								return 99;
							}
							strcpy(new_token->value.stringVal, insert_token->value.stringVal);
							new_token->type = _STRINGVALUE;
							break;
						default:
							break;
					}
					if(htable_search(g_htable->act->table, new_token) != NULL){
						destroy_all();
						return 13;
					}

					if(htable_insert(g_htable->act->table, new_token) == NULL){
						destroy_all();
						return 99;
					}
				}
				else{
					destroy_all();
					return 4; //asi 12 - nedostatecny pocet predavanych param
				}
				break;

			case I_PUSH:
				if(instr->adr1 != NULL){
					if(adr1 != NULL){
						token_queue_up(token_que, adr1);
					}
					else{
						destroy_all();
						return 5; //asi 12
					}
				}
				else{
					destroy_all();
					return 99; // asi 12
				}
				break;

			case I_DELETE:
				token_queuq_clear_all(token_que);
				break;

			case I_RETURN:
				if(instr->adr1 != NULL){
					if(adr1 == NULL){
						destroy_all();
						return 5; //error, neznam adr
					}
					if(g_return.type == _STRINGVALUE){
						free(g_return.value.stringVal);
					}
					switch(adr1->type){
						case _INTVALUE:
							g_return.value.intVal = adr1->value.intVal;
							g_return.type = _INTVALUE;
							break;

						case _DOUBLEVALUE:
							g_return.value.doubleVal = adr1->value.doubleVal;
							g_return.type = _DOUBLEVALUE;
							break;

						case _BOOLVALUE:
							g_return.value.boolVal = adr1->value.boolVal;
							g_return.type = _BOOLVALUE;
							break;

						case _STRINGVALUE:				
							if((g_return.value.stringVal = malloc(strlen(adr1->value.stringVal) + 1)) == NULL){
								destroy_all();
								return 99;
							}
							strcpy(g_return.value.stringVal, adr1->value.stringVal);
							g_return.type = _STRINGVALUE;
							break;

						case _NULLVALUE:
							g_return.type = _NULLVALUE;
							break;

						default:
							break;
					}
				}
				else{
					if(g_return.type == _STRINGVALUE){
						free(g_return.value.stringVal);
					}
					g_return.type = _NULLVALUE;
				}
				htable_list_remove_act(g_htable);
				if(instr_stack_is_empty(instruction_stack)){
					destroy_all();
					return 0; //0 uz neni kam skocit
				}
				instr_list_goto(list, instr_stack_top(instruction_stack));
				instr_stack_pop(instruction_stack);
				break;

			case I_LABEL:
				break;


	/************************** vestavene fce **************************/

			case I_BOOLVAL:
				if(token_queue_count(token_que)){
					adr1 = token_queue_remove(token_que);
					if(g_return.type == _STRINGVALUE){
						free(g_return.value.stringVal);
					}
					g_return.value.boolVal = bool_val(adr1);
					g_return.type = _BOOLVALUE;
				}
				else{
					destroy_all();
					return 4; //asi 12, zaden param
				}
				token_queuq_clear_all(token_que);
				break;

			case I_DOUBLEVAL:
				if(token_queue_count(token_que)){
					adr1 = token_queue_remove(token_que);
					if(g_return.type == _STRINGVALUE){
						free(g_return.value.stringVal);
					}
					g_return.value.doubleVal = double_val(adr1, &errno);
					if(errno){
						destroy_all();
						return errno;
					}
					g_return.type = _DOUBLEVALUE;
				}
				else{
					destroy_all();
					return 4; //asi 12, zaden param
				}
				token_queuq_clear_all(token_que);
				break;

			case I_INTVAL:
				if(token_queue_count(token_que)){
					adr1 = token_queue_remove(token_que);
					if(g_return.type == _STRINGVALUE){
						free(g_return.value.stringVal);
					}
					g_return.value.intVal = int_val(adr1);
					if(errno){
						destroy_all();
						return errno;
					}
					g_return.type = _INTVALUE;
				}
				else{
					destroy_all();
					return 4; //asi 12, zaden param
				}
				token_queuq_clear_all(token_que);
				break;

			case I_STRVAL:
				if(token_queue_count(token_que)){
					adr1 = token_queue_remove(token_que);
					if(g_return.type == _STRINGVALUE){
						free(g_return.value.stringVal);
					}
					g_return.value.stringVal = str_val(adr1, &errno);
					if(errno){
						destroy_all();
						return errno;
					}
					g_return.type = _STRINGVALUE;
				}
				else{
					destroy_all();
					return 4; //asi 12, zaden param
				}
				token_queuq_clear_all(token_que);
				break;

			case I_GET_STRING:
				token_queuq_clear_all(token_que);
				if(g_return.type == _STRINGVALUE){
					free(g_return.value.stringVal);
				}
				if((g_return.value.stringVal = get_string()) == NULL){
					destroy_all();
					return 99; //nepodarilo se alokovat
				}
				g_return.type = _STRINGVALUE;
				break;

			case I_PUT_STRING:
				if(token_queue_count(token_que)){
					count_params = 0;
					while(token_queue_count(token_que)){
						count_params = count_params + 1;
						insert_token = token_queue_remove(token_que);/////////////////////////////////////////////////////////////////////////////
						if((temp_string_destroy = str_val(insert_token, &errno)) == NULL){
							destroy_all();
							return errno;
						}
						printf("%s",temp_string_destroy);
						free(temp_string_destroy);
					}
					if(g_return.type == _STRINGVALUE){
						free(g_return.value.stringVal);
					}
					g_return.value.intVal = count_params;
					g_return.type = _INTVALUE;
				}
				else{
					if(g_return.type == _STRINGVALUE){
						free(g_return.value.stringVal);
					}
					g_return.value.intVal = 0;
					g_return.type = _INTVALUE;
				}
				break;

			case I_STRLEN:
				if(token_queue_count(token_que)){
					if((adr1 = token_queue_remove(token_que)) == NULL){
						destroy_all();
						return 4; //asi 12, nic nebylo predano
					}
					if(g_return.type == _STRINGVALUE){
						free(g_return.value.stringVal);
					}
					g_return.value.intVal = str_len(adr1, &errno);
					g_return.type = _INTVALUE;
					if(errno){
						destroy_all();
						return errno;
					}	
				}
				else{
					destroy_all();
					return 4; //asi 12, zadny param
				}
				token_queuq_clear_all(token_que);
				break;

			case I_GET_SUBSTRING:
				if(token_queue_count(token_que)){
					adr1 = token_queue_remove(token_que);
					adr2 = token_queue_remove(token_que);
					adr3 = token_queue_remove(token_que);
					if(adr1 == NULL || adr2 == NULL || adr3 == NULL){
						destroy_all();
						return 4; //asi 12, nedostatecny pocet parametru
					}

					//tuto pokracuj!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					if(g_return.type == _STRINGVALUE){
						free(g_return.value.stringVal);
					}
					temp_string_destroy = str_val(adr1, &errno);
					if(errno){
						destroy_all();
						return errno;
					}
					g_return.value.stringVal = get_substring(temp_string_destroy, int_val(adr2), int_val(adr3), &errno);
					g_return.type = _STRINGVALUE;
					free(temp_string_destroy);
					//pokud funkce neprobehne v poradku
					if(errno){
						destroy_all();
						return errno;
					}
				}
				else{
					destroy_all();
					return 4; //asi 12, zadny param
				}
				token_queuq_clear_all(token_que);
				break;

			case I_FIND_STRING:
				if(token_queue_count(token_que)){
					adr1 = token_queue_remove(token_que);
					adr2 = token_queue_remove(token_que);
					if(adr1 == NULL || adr2 == NULL){
						destroy_all();
						return 4; //asi 12, nedostatecny pocet parametru
					}
					if(g_return.type == _STRINGVALUE){
						free(g_return.value.stringVal);
					}
					temp_string_destroy = str_val(adr1, &errno);
					temp_string_destroy2 = str_val(adr2, &errno);
					if(errno){
						destroy_all();
						return 99;
					}
					g_return.value.intVal = find_string(temp_string_destroy, temp_string_destroy2);
					g_return.type = _INTVALUE;
					free(temp_string_destroy);
					free(temp_string_destroy2);
				}
				else{
					destroy_all();
					return 4; //asi 12, zadny param
				}
				token_queuq_clear_all(token_que);
				break;

			case I_SORT_STRING:
				if(token_queue_count(token_que)){
					adr1 = token_queue_remove(token_que);
					if(adr1 == NULL){
						destroy_all();
						return 4; //asi 12, nedostatecny pocet parametru
					}
					temp_string_destroy = str_val(adr1, &errno);
					if(errno){
						destroy_all();
						return errno;
					}
					sort_string(temp_string_destroy);
				}
				else{
					destroy_all();
					return 4; //asi 12, zadny param
				}
				token_queuq_clear_all(token_que);
				if(g_return.type == _STRINGVALUE){
					free(g_return.value.stringVal);
				}
				g_return.value.stringVal = malloc(strlen(temp_string_destroy) + 1);
				strcpy(g_return.value.stringVal, temp_string_destroy);
				g_return.type = _STRINGVALUE;
				free(temp_string_destroy);
				break;

			default:
				break;

		}
		instr_list_next(list);
		if(new_token != NULL){
			token_list_insert(g_token_list, new_token);
		}
	}
}
