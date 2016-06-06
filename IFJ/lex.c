// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "lex.h"
#include "ext_string.h"
#include "global.h"
#include "ial.h"
#include "adt.h"
#include "keywords.h"
#include "parser.h"


/**
 * Konvertuje textovy retazec predstavujuci hexadecimalne cislo na int.
 * Pozor na pretecenie pri type int!
 * @param str retazec, ktory sa ma prekonvertovat.
 * @return vracia prekonvertovane cislo typu int v desiatkovej sustave.
 * V pripade nuloveho ukazatela na retazec vrati -1.
 */
int hexToDec(char *str) {
    int decimal = 1;
    int i = 0;

    if (str == NULL) 
	return -1; // osetrenie nuloveho ukazatelu
    while (str[i] != '\0') {
        if (str[i] >= 'a' && str[i] <= 'f') {
            str[i] = toupper(str[i]); // velke pismena zmeni na male
            }
        if (str[i] >= 'A' && str[i] <= 'F') { // relevantne pismeno hexadecimalneho zapisu
            if (i == 0) { // prva iteracia
                decimal = str[i] - 55; // 55 je rozdiel medzi hodnotou cisla predstavujuceho dany znak a znakom v ascii tabulke
            }
            else { // okrem prvej iteracie cyklu
                decimal = (decimal * 16) + (str[i] - 55); // magicka konstanta???
            }
        } // cislica
        else if (str[i] >= '0' && str[i] <= '9') {
            if (i == 0) { // 1. iteracia
                decimal = str[i] - 48; // 48 je rozdiel ascii hodnoty cislice a jej int hodnoty
            }
            else { // 2. a dalsia iteracia
                decimal = (decimal * 16) + (str[i] - 48);
            }
        }
    	i++;
    }
        return decimal; // vysledok
}


/**
 * Lexikalny analyzator - rozpoznava jednotlive lexemy (lex. jednotky) 
 * zadaneho programu, kt. je na standardnom vstupe. Z lex. jednotiek potom
 * vytvori korespondujuce tokeny, kt. budu obsahovat dolezite info pre 
 * semanticku analyzu a interpretaciu.
 * @return vracia odkaz na novy token danej lex. jednotky. V pripade chyby 
 * vrati NULL a nastavi glob. premennu internalError na lex., synt. alebo 
 * vnutornu chybu.
 */
TToken* get_token() {

	bool DEBUG = false; // pomocna premenna pre docasny vypis z konecnych stavov
	bool koniec = false; // premenna, ktora zabezpeci vychod z nekonecneho while
	bool empty; // indikuje nacitanie prazdneho retazca
	int cntrl = _BEGIN; // pociatocny stav
	int znak = 0;

	STRING out; // retazec,do kt. sa postupne budu pridavat znaky tvoriace jednotlivy lexem
	TToken *token = malloc(sizeof(TToken));
	if (token == NULL) {
			if (DEBUG) printf("Chyba alokacie tokenu.\n");
			internalError = INTERN_ERROR; // indikacia vnutornej chyby
			free(token); 
			return NULL; 
	}
	token->name = NULL;
	int error = CreateStr(&out, 0);
	if (error != 0) { // 1 = chyba pri alokaci, 2 = parameter je nealokovany alebo NULL
		if (DEBUG) printf("Chyba pri alokacii vystupneho retazca.\n");
		internalError = INTERN_ERROR; // indikacia vnutornej chyby
		free(token);
		FreeStr(&out);
		return NULL;
	}

	while(1) {
		switch(cntrl) {
			case _BEGIN: // _BEGIN
				koniec = false;
				znak = fgetc(soubor);
				if (is_begin == true && znak != '<') {
					cntrl = _ERROR;
					continue;
				}

				if (!isspace(znak)) AddStrChar(&out, znak); //vlozi nebiely znak
				if (isalpha(znak) || (znak == '_')) { // alpha || _
					cntrl = _T_FUNC; 					
					continue;
				}
				else if (isdigit(znak)) { // [0-9]
					if (znak == '0') {
						cntrl = _T_ZERO;
						continue;
					}
					else { // [1-9]
						cntrl = _T_NUM;
						continue;
					}
				}
				else if (isspace(znak)) { // biely znak
					cntrl = _BEGIN;
						continue;
				}

				switch(znak) {
					case '"':
						cntrl = _T_STR;
						RemoveStrChar(&out); // vymaze uvodne uvodzovky
						empty = true; // indikuje, ci bude nacitany prazdny retazec
						break;
					case EOF: 
						cntrl = _EOF;
						break;
					case '{':
						cntrl = _LBRACE;
						break;
					case '}':
						cntrl = _RBRACE;
						break;
					case '!':
						cntrl = _T_NEG;
						break;
					case '+': 
						cntrl = _PLUS;
						break;
					case '-':
						cntrl = _MINUS;
						break;
					case '*':
						cntrl = _MULTI;
						break;
					case '/':
						cntrl = _T_DIV;
						break;
					case '.':
						cntrl = _DOT;
						break;
					case ',':
						cntrl = _COMMA;
						break;
					case ';':
						cntrl = _SEMICOLON;
						break;
					case '(':
						cntrl = _LPARENTHESE;
						break;
					case ')':
						cntrl = _RPARENTHESE;
						break;
					case '>':
						cntrl = _T_GREATER;
						break;
					case '<':
						cntrl = _T_LESS;
						break;
					case '=':
						cntrl = _T_EQ;
						break;
					case '$':
						cntrl = _T_VAR;
						break;
					default:
						cntrl = _ERROR;
						break; 
				}

				break; // end _BEGIN


			case _T_VAR: // _T_VAR
				znak = fgetc(soubor);
				AddStrChar(&out, znak);
				
				if (isalpha(znak) || znak == '_') { // alpha || _
					cntrl = _T_VARNAME;
					continue;
				}
				else {
					cntrl = _ERROR;
					continue; 
				}
				
				break; // end _T_VAR

			case _T_VARNAME: // _T_VARNAME
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (isalnum(znak) || (znak == '_')) { // alnum || _
					cntrl = _T_VARNAME;
					continue;
				}	
				else {
					cntrl = _VARIABLE;
					continue;
				}

				break; // end _T_VARNAME

			case _T_NEG: // _T_NEG
				znak = fgetc(soubor);	
				AddStrChar(&out, znak);

				if (znak == '=') {
					cntrl = _T_NEG_EQ;
					continue;
				}
				else {
					cntrl = _ERROR;
					continue;
				}

				break; // end _T_NEG

			case _T_NEG_EQ: // _T_NEG_EQ 
				znak = fgetc(soubor);
				AddStrChar(&out, znak);
				
				if (znak == '=') {
					cntrl = _N3EQ;
					continue;
				}
				else {
					cntrl = _ERROR;
					continue;
				}

				break; // end _T_NEG_EQ

			case _T_GREATER: // _T_ GREATER
				znak = fgetc(soubor);
				AddStrChar(&out, znak);
				
				if (znak == '=') {
					cntrl = _GEQ;
					continue;
				}
				else {
					cntrl = _GREATER;
					continue;
				}

				break; // end _T_GREATER

			case _T_LESS: // _T_LESS
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				switch(znak) {
					case '=':
						cntrl = _LEQ;
						break;
					case '?':
						cntrl = _T_LESS_QST;
						break;
					default:
						cntrl = _LESS;
						break;
				}

				break; // end _T_LESS

			case _T_EQ: // _T_EQ
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (znak == '=') {
					cntrl = _T_EQ_EQ;
					continue;
				}
				else {
					cntrl = _EQUAL;
					continue; 
				}

				break; // end _T_EQ

			case _T_EQ_EQ: // _T_EQ_EQ
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (znak == '=') {
					cntrl = _3EQ;
					continue;
				}
				else {
					cntrl = _ERROR;
					continue; 
				}

				break; // end _T_EQ_EQ

			case _T_STR: // _T_STR
				znak = fgetc(soubor);
				if (znak != '"') {
					AddStrChar(&out, znak); // vlozi vsetko okrem "
					empty = false; // nacitany retazec nie je prazdny
				}

				if (znak == 92) { // backslash
					cntrl = _T_ESC;
					continue;
				}
				else if (znak == '"') {
					cntrl = _STRINGVALUE;
					continue;
				}
				else if ((znak == EOF) || (znak == '$') || (znak <= 31)) { 
					cntrl = _ERROR;
					continue;
					}
				else { // znak >= 32
					cntrl = _T_STR;
					continue;
				}

				break; // end _T_STR

			case _T_ESC: // _T_ESC
				znak = fgetc(soubor);

				//    \\ = \        \$ = $         \" = "
				if (znak == 92 || znak == '$' || znak == '"') { //  \ || $ || "
					cntrl = _T_STR;
					RemoveStrChar(&out); // vymaze prve lomitko
					AddStrChar(&out,znak); // vlozi konkretny znak reprezentovany danou esc sekvenciou
					continue;
				}
				//      \n = new line     \t = tab
				else if (znak == 'n' || znak == 't') { // new_line || tab
					cntrl = _T_STR;
					RemoveStrChar(&out); // vymaze prve lomitko
					if (znak == 'n') AddStrChar(&out,10);//vlozi novy riadok
					else AddStrChar(&out,9); // vlozi horizontalny tabulator
					continue;
				}
				else if (znak == 'x') { // \x
					cntrl = _T_HEX;
					AddStrChar(&out,znak);
					continue;
				}
				else if (znak <= 31 || znak == EOF) { 
					cntrl = _ERROR;
					continue;
				}
				else { //znak >=32
					cntrl = _T_STR;					
					AddStrChar(&out,znak); // normalne pokracovanie stringu
					continue;
				}

				break; // end _T_ESC

			case _T_HEX: // _T_HEX
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if ((isdigit(znak)) || (znak >= 'A' && znak <= 'F') || (znak >= 'a' && znak <= 'f')) {
					cntrl = _T_HEX_AFNUM;
					continue;
				}
				else if (znak == '"') {
					cntrl = _STRINGVALUE; // koncovy stav string
					RemoveStrChar(&out);
					continue;
				}
				else if (znak == 92) { // backslash
					cntrl = _T_ESC;
					continue;
				}
				else if (znak == EOF || znak <= 31 || znak == '$') {
					cntrl = _ERROR;
					continue;
				}
				else { // znak >= 32
					cntrl = _T_STR;
					continue;
				}

				break; // end _T_HEX

			case _T_HEX_AFNUM: // _T_HEX_AFNUM
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if ((isdigit(znak)) || (znak >= 'A' && znak <= 'F') || (znak >= 'a' && znak <= 'f')) {
					cntrl = _T_HEX_AFNUM_AFNUM;
					continue;
				}
				else if (znak == '"') {
					cntrl = _STRINGVALUE; // koncovy stav string
					RemoveStrChar(&out);
					continue;
				}
				else if (znak == 92) { // backslash
					cntrl = _T_ESC;
					continue;
				}
				else if (znak == EOF || znak <= 31 || znak == '$') {
					cntrl = _ERROR;
					continue;
				}
				else { // znak >= 32
					cntrl = _T_STR;
					continue;
				}

				break; // end _T_HEX_AFNUM

			case _T_HEX_AFNUM_AFNUM: // _T_HEX_AFNUM_AFNUM
				znak = fgetc(soubor);

				if (znak == 92) { // backslash
					cntrl = _T_ESC;
					int decimal = hexToDec(&out.str[out.length-2]);
					// ak decimal == -1, tak bol predany nulovy ukazatel funkcii - tu sa to nestane
					RemoveStrChar(&out);
					RemoveStrChar(&out);
					RemoveStrChar(&out);
					RemoveStrChar(&out); // odstrani \xdd
					AddStrChar(&out, decimal); // znak hexadecimalny zameni za decimalny
					AddStrChar(&out, znak); // prida nacitany backslash do stringu
					continue;
				}
				else if (znak == '"') { // string "... \xdd"
					cntrl = _STRINGVALUE; // koncovy stav string
					int decimal = hexToDec(&out.str[out.length-2]); 
					// ak decimal == -1, tak bol predany nulovy ukazatel funkcii - tu sa to nestane
					RemoveStrChar(&out);
					RemoveStrChar(&out);
					RemoveStrChar(&out);
					RemoveStrChar(&out); // odstrani \xdd
					AddStrChar(&out, decimal); // znak hexadecimalny zameni za decimalny
					continue;
				}
				else if (znak == EOF || znak <= 31 || znak == '$') {
					cntrl = _ERROR;
					continue;
				}
				else { // znak >= 32
					cntrl = _T_STR;
					int decimal = hexToDec(&out.str[out.length-2]);
					// ak decimal == -1, tak bol predany nulovy ukazatel funkcii - tu sa to nestane
					RemoveStrChar(&out);
					RemoveStrChar(&out);
					RemoveStrChar(&out);
					RemoveStrChar(&out); // odstrani \xdd 
					AddStrChar(&out,decimal); // znak hexadec. zameni za decimalny
					AddStrChar(&out, znak); // prida nacitany znak do stringu
					continue;
				}

				break; // end _T_HEX_AFNUM_AFNUM

			case _T_NUM: // _T_NUM
				znak = fgetc(soubor);
				AddStrChar(&out, znak);
	
				if (znak == '.') {
					cntrl = _T_DOUBLE;
					continue;
				}
				else if (znak == 'e' || znak == 'E') {
					cntrl = _T_DOUBLE_EXP;
					continue;
				}
				else if (isdigit(znak)) {
					cntrl = _T_NUM;
					continue;
				}
				else {
					cntrl = _INTVALUE;
					continue;
				}

				break; // end _T_NUM

			case _T_DOUBLE: // _T_DOUBLE
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (isdigit(znak)) { // [0-9]
					cntrl = _T_DOUBLE_NUM;
					continue;
				}
				else {
					cntrl = _ERROR;
					continue;
				}

				break; // end _T_DOUBLE

			case _T_DOUBLE_NUM: // _T_DOUBLE_NUM
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (isdigit(znak)) { // [0-9]
					cntrl = _T_DOUBLE_NUM;
					continue;
				}
				else if (znak == 'e' || znak == 'E') {
					cntrl = _T_DOUBLE_EXP;
					continue;
				}
				else {
					cntrl = _DOUBLEVALUE;
					continue;
				}

				break; // end _T_DOUBLE_NUM

			case _T_DOUBLE_EXP: // _T_DOUBLE_EXP
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (isdigit(znak)) { // [0-9]
					cntrl = _T_DOUBLE_PM_NUM;
					continue;
				}
				else if (znak == '+' || znak == '-') {
					cntrl = _T_DOUBLE_PM;
					continue;
				}
				else {
					cntrl = _ERROR;
					continue;
				}

				break; // end _T_DOUBLE_EXP

			case _T_DOUBLE_PM: // _T_DOUBLE_PM
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (isdigit(znak)) { // [0-9]
					cntrl = _T_DOUBLE_PM_NUM;
					continue;
				}
				else {
					cntrl = _ERROR;
					continue;
				}

				break; // end _T_DOUBLE_PM

			case _T_DOUBLE_PM_NUM: // _T_DOUBLE_PM_NUM
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (isdigit(znak)) { // [0-9]
					cntrl = _T_DOUBLE_PM_NUM; //smycka sama do seba
					continue;
				}
				else {
					cntrl = _DOUBLEVALUE; //koncovy stav = double
					continue;
				}

				break; // end _T_DOUBLE_PM_NUM

			case _T_DIV: // _T_DIV
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (znak == '/') { // //
					cntrl = _T_DIV_DIV;
					continue;
				}
				else if (znak == '*') { // /*
					cntrl = _T_DIV_MULTI;
					continue;
				}
				else {
					cntrl = _DIV; //koncovy stav = deleno
					continue;
				}

				break; // end _T_DIV

			case _T_DIV_DIV: // _T_DIV_DIV = riadkovy komentar
				znak = fgetc(soubor);

				if (znak == 10) { // znak noveho riadku = '\n'
					cntrl = _BEGIN; //chod na zaciatok, akoby sa nic nestalo
					RemoveStrChar(&out); // vymaz obe lomitka komentarove
					RemoveStrChar(&out);
					if (is_begin == true) {
						internalError = SYNTAX_ERROR;
						///////////
						FreeStr(&out);
						free(token);
						//////////
						return NULL;
					}
					continue;
				}
				else if (znak == EOF) {
					cntrl = _EOF; //koncovy stav = koniec suboru
					RemoveStrChar(&out);
					RemoveStrChar(&out); // vymaz obe lomitka komentarove
					AddStrChar(&out,znak); // nacitaj EOF
					if (is_begin == true) {
						internalError = SYNTAX_ERROR;
						////////////
						FreeStr(&out);
						free(token);
						////////////
						return NULL;
					}
					continue;
				}
				else {
					cntrl = _T_DIV_DIV; // smycka sama do seba
					continue;
				}

				break; // end _T_DIV_DIV

			case _T_DIV_MULTI: // _T_DIV_MULTI = zaciatok blok. komentaru
				znak = fgetc(soubor);
				RemoveStrChar(&out);
				RemoveStrChar(&out); // vymaze zapisane /*

				if (znak == '*') { // /*...*
					cntrl = _T_DIV_MULTI_MULTI;
					continue;
				}
				else if (znak == EOF) {
					cntrl = _ERROR; // nebol ukonceny blokovy komentar
					continue;
				}
				else {
					cntrl = _T_DIV_MULTI; //smycka sama do seba
					continue;
				}

				break; // end _T_DIV_MULTI

			case _T_DIV_MULTI_MULTI: // _T_DIV_MULTI_MULTI
				znak = fgetc(soubor);


				if (znak == '/') { // koniec blokoveho komentaru /*...*/
					cntrl = _BEGIN; // chod na zaciatok, akoby nic
					if (is_begin == true) {
						internalError = SYNTAX_ERROR;
						////////////////////
						FreeStr(&out);
						free(token);
						////////////////////
						return NULL;
					}
					continue;
				}
				else if (znak == EOF) { // nebol ukonceny blokovy komentar
					cntrl = _ERROR;					
					if (is_begin == true) {
						internalError = SYNTAX_ERROR;
						////////////////////
						FreeStr(&out);
						free(token);
						////////////////////
						return NULL;
					}
					continue;
				}
				else { // pokracovanie blokoveho komentaru
					cntrl = _T_DIV_MULTI; // /*...
					continue;
				}

				break; // end _T_DIV_MULTI_MULTI

			case _T_LESS_QST: // _T_LESS_QST
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (znak == 'p') { // <?p
					cntrl = _T_LESS_QST_P;
					continue;
				}
				else {
					cntrl = _ERROR;
					continue;
				}

				break; // end _T_LESS_QST

			case _T_LESS_QST_P: // _T_LESS_QST_P
				znak = fgetc(soubor);
				AddStrChar(&out, znak);
	
				if (znak == 'h') { // <?ph
					cntrl = _T_LESS_QST_P_H;
					continue;
				}
				else {
					cntrl = _ERROR;
					continue;
				}

				break; // end _T_LESS_QST_P

			case _T_LESS_QST_P_H: // _T_LESS_QST_P_H
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (znak == 'p') { // <?php
					cntrl = _T_LESS_QST_P_H_P;
					continue;
				}
				else {
					cntrl = _ERROR;
					continue;
				}

				break; // end _T_LESS_QST_P_H

			case _T_LESS_QST_P_H_P: // _T_LESS_QST_P_H_P
				znak = fgetc(soubor);
				AddStrChar(&out, znak);
				
				if (isspace(znak)) {
					cntrl = _MAIN; // koncovy stav = <?php  (biely znak na konci)
					continue;
				}
				else if (znak == EOF) {
					cntrl = _MAIN;
					ungetc(znak,soubor); // vrati EOF do streamu
					RemoveStrChar(&out); // odstrani EOF z konce
				}
				else {
					cntrl = _ERROR;
					continue;
				}
				
				break; // end _T_LESS_QST_P_H_P

			case _T_ZERO: // _T_ZERO
				znak = fgetc(soubor);
				RemoveStrChar(&out); // odstrani pociatocnu nulu

				if (isdigit(znak)) {
					if (znak == '0') { // 0
						cntrl = _T_ZERO; // smycka sama do seba
						AddStrChar(&out, znak);
						continue;
					}
					else { // [1-9]
						cntrl = _T_NUM;
						AddStrChar(&out, znak);
						continue;
					}
				}
				else if (znak == 'e' || znak == 'E') { // 0e || 0E
					cntrl = _T_DOUBLE_EXP;
					AddStrChar(&out, '0');
					AddStrChar(&out, znak);
					continue;
				}
				else if (znak == '.') {
					cntrl = _T_DOUBLE;
					AddStrChar(&out, '0');
					AddStrChar(&out, znak);
					continue;
				}
				else {
					cntrl = _INTVALUE; // koncovy stav = integer
					AddStrChar(&out, '0');
					AddStrChar(&out, znak);
					continue;
				}

				break; // end _T_ZERO

			case _T_FUNC: // _T_FUNC
				znak = fgetc(soubor);
				AddStrChar(&out, znak);

				if (isalnum(znak) || znak == '_') { // alnum || _
					cntrl = _T_FUNC; // smycka sama do seba
					continue;
				}
				else {
					cntrl = _NAMEFUNCT; // koncovy stav = ident.fcie/kluc.slovo/konstanta
					continue;
				}

				break; // end _T_FUNC

			case _ERROR: // _ERROR
				FreeStr(&out); // uvolni STRING out z pamate
				if (DEBUG) printf("Lexikalna chyba!\n");
				koniec = true; // narazil som na lex. chybu, skonci analyzu
	
				if (is_begin == true) 
					internalError = SYNTAX_ERROR; // syntakt. chyba 
				else 
					internalError = LEX_ERROR; // lex. chyba
				////////////////////
				free(token); //
				////////////////////
				return NULL; // chyba
				break; // end _ERROR

// nahore jsou prechodne stavy ... dole jsou koncove

			case _EQUAL: // _EQUAL
				ungetc(znak,soubor);
				RemoveStrChar(&out);
				if (DEBUG) printf("=");
				koniec = true; // koncovy stav, lex.analyza moze skoncit
				token->type = _EQUAL;
				break; // end _EQUAL
			
			case _MAIN: // _MAIN
				if (DEBUG) printf("<?php");
				koniec = true;	
				is_begin = false; // odteraz sa LA moze skoncit uspesne - bez synt.chyby
				token->type = _MAIN;
				break; // end _MAIN

			case _LBRACE: // _LBRACE
				if (DEBUG) printf("{");
				koniec = true;
				token->type = _LBRACE; 
				break; // end _LBRACE

			case _RBRACE: // _RBRACE
				if (DEBUG) printf("}");
				koniec = true;
				token->type = _RBRACE; 
				break; // end _RBRACE

			case _LPARENTHESE: // _LPARENTHESE
				if (DEBUG) printf("(");
				koniec = true;
				token->type = _LPARENTHESE; 
				break; // end _LPARENTHESE

			case _RPARENTHESE: // _RPARENTHESE
				if (DEBUG) printf(")");
				koniec = true;
				token->type = _RPARENTHESE; 
				break; // end _RPARENTHESE

			case _SEMICOLON: // _SEMICOLON
				if (DEBUG) printf(";");
				koniec = true;
				token->type = _SEMICOLON; 
				break; // end _SEMICOLON

			case _COMMA: // _COMMA
				if (DEBUG) printf(",");
				koniec = true;
				token->type = _COMMA; 
				break; // end _COMMA

			case _DOT: // _DOT
				if (DEBUG) printf(".");
				koniec = true;
				token->type = _DOT; 
				break; // end _DOT

			case _NAMEFUNCT: //_NAMEFUNCT
				ungetc(znak,soubor);
				RemoveStrChar(&out);
								
				if (strcmp(out.str,"true") == 0) {
					cntrl = _TRUE;
					continue;
				}
				else if (strcmp(out.str,"false") == 0) {
					cntrl = _FALSE;
					continue;
				}
				else if (strcmp(out.str,"null") == 0) {
					cntrl = _NULLVALUE;
					continue;
				}
				else if (strcmp(out.str,"if") == 0) {
					cntrl = _IF;
					continue;
				}
				else if (strcmp(out.str,"else") == 0) {
					cntrl = _ELSE;
					continue;
				}
				else if (strcmp(out.str,"while") == 0) {
					cntrl = _WHILE;
					continue;
				}
				else if (strcmp(out.str,"return") == 0) {
					cntrl = _RETURN;
					continue;
				}
				else if (strcmp(out.str,"for") == 0) {
					cntrl = _FOR;
					continue;
				}
				else if (strcmp(out.str,"break") == 0) {
					cntrl = _BREAK;
					continue;
				}
				else if (strcmp(out.str,"continue") == 0) {
					cntrl = _CONTINUE;
					continue;
				}
				else if (strcmp(out.str,"function") == 0) {
					cntrl = _FUNCT;
					continue;
				}
				else { // identifikator funkcie
					if (DEBUG) printf("meno_funkcie");
					koniec = true;
					token->type = _NAMEFUNCT;
					token->name = out.str;
				}

				break; // end _NAMEFUNCT 

			case _LEQ: // _LEQ
				if (DEBUG) printf("<=");
				koniec = true;
				token->type = _LEQ;
				break; // end _LEQ

			case _GEQ: // _GEQ
				if (DEBUG) printf(">=");
				koniec = true;
				token->type = _GEQ;
				break; // end _GEQ

			case _3EQ: // _3EQ
				if (DEBUG) printf("===");
				koniec = true;
				token->type = _3EQ;
				break; // end _3EQ

			case _N3EQ: // _N3EQ
				if (DEBUG) printf("!==");
				koniec = true;
				token->type = _N3EQ;
				break; // end _N3EQ

			case _PLUS: // _PLUS
				if (DEBUG) printf("+");
				koniec = true;
				token->type = _PLUS; 
				break; // end _PLUS

			case _MINUS: // _MINUS
				if (DEBUG) printf("-");
				koniec = true;
				token->type = _MINUS; 
				break; // end _MINUS

			case _MULTI: // _MULTI
				if (DEBUG) printf("*");
				koniec = true;
				token->type = _MULTI; 
				break; // end _MULTI

			case _DIV: // _DIV
				ungetc(znak,soubor);
				RemoveStrChar(&out);
				if (DEBUG) printf("/");
				koniec = true;
				token->type = _DIV; 
				break; // end _DIV

			case _VARIABLE: // _VARIABLE
				ungetc(znak,soubor);
				RemoveStrChar(&out);
				if (DEBUG) printf("$premenna");
				koniec = true;
				token->type = _VARIABLE;
				token->name = out.str;
				break; // end _VARIABLE

			case _INTVALUE: // _INTVALUE
				ungetc(znak,soubor);
				RemoveStrChar(&out);
				if (DEBUG) printf("integer");
				koniec = true;
				token->type = _INTVALUE; 
				int integer = atoi(out.str);
				token->value.intVal = integer;
				break; // end _INTVALUE

			case _DOUBLEVALUE: // _DOUBLEVALUE
				ungetc(znak,soubor);
				RemoveStrChar(&out);
				if (DEBUG) printf("double");
				koniec = true;
				token->type = _DOUBLEVALUE; 
				double dbl = atof(out.str);
				token->value.doubleVal = dbl;
				break; // end _DOUBLEVALUE

			case _STRINGVALUE: // _STRINGVALUE
				koniec = true;
				token->type = _STRINGVALUE;

				if (empty) { // prazdny retazec
					if (DEBUG) printf("prazdny retazec.\n");
					token->value.stringVal = out.str;
				}
				else { // neprazdny retazec
					if (DEBUG) printf("string"); 
					token->value.stringVal = out.str;
				}
				break; // end _STRINGVALUE

			case _LESS: // _LESS
				ungetc(znak,soubor);
				RemoveStrChar(&out);
				if (DEBUG) printf("<");
				koniec = true;
				token->type = _LESS; 
				break; // end _LESS

			case _GREATER: // _GREATER
				ungetc(znak,soubor);
				RemoveStrChar(&out);
				if (DEBUG) printf(">");
				koniec = true;
				token->type = _GREATER; 
				break; // end _GREATER

			case _EOF: // _EOF
				if (DEBUG) printf("EOF");
				koniec = true;
				token->type = _EOF; 
				break; // end _EOF

			case _TRUE: // _TRUE
				if (DEBUG) printf("true");
				koniec = true;
				token->type = _BOOLVALUE;
				token->value.boolVal = true;
				break; // end _TRUE

			case _FALSE: // _FALSE
				if (DEBUG) printf("false");
				koniec = true;
				token->type = _BOOLVALUE;
				token->value.boolVal = false;
				break; // end _FALSE
				
			case _NULLVALUE: // _NULLVALUE
				if (DEBUG) printf("null");
				koniec = true;
				token->type = _NULLVALUE;
				break; // end _NULLVALUE

			case _IF: // _IF
				if (DEBUG) printf("if");
				koniec = true;
				token->type = _IF;
				break; // end _IF

			case _ELSE: // _ELSE
				if (DEBUG) printf("else");
				koniec = true;
				token->type = _ELSE;
				break; // end _ELSE

			case _WHILE: // _WHILE
				if (DEBUG) printf("while");
				koniec = true;
				token->type = _WHILE;
				break; // end _WHILE

			case _FUNCT: // _FUNCT
				if (DEBUG) printf("function");
				koniec = true;
				token->type = _FUNCT;
				break; // end _FUNCT

			case _RETURN: // _RETURN
				if (DEBUG) printf("return");
				koniec = true;
				token->type = _RETURN;
				break; // end _RETURN

			case _FOR: // _FOR
				if (DEBUG) printf("for");
				koniec = true;
				token->type = _FOR;
				break; // end _FOR

			case _BREAK: // _BREAK
				if (DEBUG) printf("break");
				koniec = true;
				token->type = _BREAK;
				break; // end _BREAK

			case _CONTINUE: // _CONTINUE
				if (DEBUG) printf("continue");
				koniec = true;
				token->type = _CONTINUE;
				break; // end _CONTINUE

			default: if (DEBUG) printf("default???"); // sem sa nedostane
				break;

		} // end switch
	if (koniec) break; // ak analyza skoncila, mozme vyjst z cyklu
	} // end while

//	printf("out=\"%s\"\n",out.str);
	if (is_begin == true) { // vstupny program nazacina korektne znackou <?php
		internalError = SYNTAX_ERROR; // synt.chyba
		///////////////////
		FreeStr(&out); // uvolni STRING out, lebo sa nepriradi
		free(token);
		//////////////////
		return NULL;
	}

	if ((token->type != _VARIABLE) && (token->type != _NAMEFUNCT) && (token->type != _STRINGVALUE)) { // premenna alebo id. funkcie
		FreeStr(&out); // uvolni STRING out, lebo sa nepriradi
	}
	token_list_insert(g_token_list,token); // vlozi kazdy token do zoznamu tokenov
	return token;
}
