// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

//################################################
// Nazev: main.c                                 #
// Popis: zpracuje pramatery a spusti prekladac  #
//################################################

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "generator.h"
#include "instruction.h"
#include "interpret.h"
#include "parser.h"
#include "adt.h"
//#include "ial.h"

const int HTSIZE = 128; // max. hash tabulek

//##########################################
// Nazev: init                             #
// Popis: inicializuje potrebne promenne   #
//##########################################
void init() {
	internalError = 0; // interni chyba
	is_begin = true; // pro <?php lex. analyzu
	// globalni seznam TS prom
	g_htable = htable_list_init();
	htable_list_insert(g_htable, htable_init(HTSIZE));
	// globalni seznam instrukci
	g_instr = instr_list_init();
	// globalni seznam tokenu
	g_token_list = token_list_init();
	// globalni token pro return v intepretu
	g_return.name = NULL;
	g_return.type = _NULLVALUE;
}

//##############################
// Nazev: init_internal_fce    #
// Popis: prida polozku do TS  #
//##############################
void init_internal_fce(char* name, int value) {

	TToken* help = malloc(sizeof(TToken)); 
	token_list_insert(g_token_list, help);
	help->type = _NAMEFUNCT;
	help->name = malloc(strlen(name) + 1);
	strcpy(help->name, name); 
	help->value.intVal = value;
	help->isDefine = true;
	htable_insert(g_htable->first->table, help); // vlozit do TS
}

//############################
// Nazev: main               #
// Popis: start prekaladace  #
//############################
int main(int argc, char* argv[]) {

	if (argc <= 1) {
		fprintf(stderr, "Prosim zadejte jako parametr soubor.\n");
		return 0;
	}
	else {
		soubor = fopen(argv[1],"r");
		if (soubor == NULL) {
			fprintf(stderr, "Soubor %s nebyl nalezen.\n", argv[1]);
			return 0;
		}
	}

	// pocatecni init
	init();
	// vlozeni vestavenych fci
	init_internal_fce("boolval", 1);
	init_internal_fce("doubleval", 1);
	init_internal_fce("intval", 1);
	init_internal_fce("strval", 1);
	init_internal_fce("get_string", 0);
	init_internal_fce("get_substring", 3);
	init_internal_fce("put_string", 0);
	init_internal_fce("strlen", 1);
	init_internal_fce("get_strlen", 3);
	init_internal_fce("find_string", 2);
	init_internal_fce("sort_string", 1);  
	
	int OK = parser(); // zavolame parser
	if (OK != 0) { // nastala chyba? opravdu je OK na 1 ???
		if (internalError == 0) { // nastala chyba v lexu?
			internalError = OK; // nenastala
		}
		switch(internalError)
		{
			case 1: 
				// lexikalni chyba
				fprintf(stderr, "Chyba cislo 1!\n");
				break;
			case 2:
				// syntakticka chyba
				fprintf(stderr, "Chyba cislo 2!\n");
				break;
			case 3:
				// semanticka chyba
				fprintf(stderr, "Chyba cislo 3!\n");
				break;
			case 4: 
				// chybejici paramter pri volani fce
				fprintf(stderr, "Chyba cislo 4!\n");
				break;
			case 99:
				// interni chyba
				fprintf(stderr, "Interni chyba!\n");
				break;
		}
		// uvolneni vsech polozek
		htable_list_free(g_htable);
		instr_list_free(g_instr);
		token_list_clear_all(g_token_list);
		token_list_free(g_token_list);
		fclose(soubor);
		return internalError; // navrat chyby
	}

	internalError = interpret(g_instr); // zavolame interpret

	if (internalError) { // 0 -> OK
		switch(internalError)
		{
			case 4: 
				// chybejici paramter pri volani fce
				fprintf(stderr, "Chyba cislo 4!\n");
				break;
			case 5:
				// nedeklarovana promenna
				fprintf(stderr, "Chyba cislo 5!\n");
				break;
			case 10:
				// deleni nulou
				fprintf(stderr, "Chyba cislo 10!\n");
				break;
			case 11:
				// chyba pri pretypovani
				fprintf(stderr, "Chyba cislo 11!\n");
				break;
			case 12:
				// typova nekompatibilita
				fprintf(stderr, "Chyba cislo 12!\n");
				break;
			case 13:
				// ostatni semanticke chyby
				fprintf(stderr, "Chyba cislo 13!\n");
				break;
			case 99:
				// interni chyba
				fprintf(stderr, "Interni chyba!\n");
				break;
		}
	}
	// standardni uvolneni polozek
	htable_list_free(g_htable);
	instr_list_free(g_instr);
	token_list_clear_all(g_token_list);
	token_list_free(g_token_list);
	fclose(soubor);

	return internalError; 
} // konec main
