// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#ifndef _GLOBAL_H
#define _GLOBAL_H
#include "adt.h"
#include "ial.h"
#include "stdio.h"

int internalError; // glob. premenna na indikaciu vnutornej chyby

TlistHtable* g_htable;   // glob. hash tabulka pre ukladanie potrebnych tokenov
// je toto potreba TlistToken * list ?x#&!!

TlistInstr* g_instr; // glob. seznam instrukci pro generator a interpret

TlistToken* g_token_list;  // glob. zoznam pre ukladanie (ne)potrebnych tokenov

bool is_begin;     // glob. premnenna indikuje nacitanie znacky "<?php" vo vstupnom programe

TToken g_return; // glob. promenna token pro return v interpretu

FILE* soubor;
#endif
