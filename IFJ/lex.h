// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#ifndef LEXWORDS
#define LEXWORDS
// ak je nieco v gulatych zatvorkach(), je to volitelne (najma pri vyrazoch v komentaroch)

#include "ial.h"

#define _BEGIN 100 // pocatecny stav
#define _T_VAR 101 // $
#define _T_VARNAME 102 // $ alpha|_  ||  alphanum|_
#define _T_NEG 103 // !
#define _T_NEG_EQ 104 // ! =
#define _T_GREATER 105 // >
#define _T_LESS 106 // <
#define _T_EQ 107 // =
#define _T_EQ_EQ 108 // = =
#define _T_STR 109 // " || " ASCII>=32
#define _T_ESC 110 // " \ musi_tu_byt_nejaky_nepodstatny_text_skrz_backslash
#define _T_HEX 111 // " \ x
#define _T_HEX_AFNUM 112 // " \ x [A-F]num
#define _T_HEX_AFNUM_AFNUM 113 // " \ x [A-F]num [A-F]num
#define _T_NUM 114 // [1-9]
#define _T_DOUBLE 115 // num .
#define _T_DOUBLE_NUM 116 // num . num
#define _T_DOUBLE_EXP 117 // num . num e|E || num e|E
#define _T_DOUBLE_PM 118 // num . num e|E +|- || num e|E +|-
#define _T_DOUBLE_PM_NUM 119 // num . num e|E (+|-) num || num e|E (+|-) num
#define _T_DIV 120 // /
#define _T_DIV_DIV 121 // / /
#define _T_DIV_MULTI 122 // / *
#define _T_DIV_MULTI_MULTI 123 // / * *
#define _T_LESS_QST 124 // < ?
#define _T_LESS_QST_P 125 // < ? p
#define _T_LESS_QST_P_H 126 // < ? p h
#define _T_LESS_QST_P_H_P 127 // < ? p h p
#define _T_ZERO 128 // 0
#define _T_FUNC 129 // alpha|_  || alphanum|_

#define _ERROR 199

TToken* get_token();

#endif

