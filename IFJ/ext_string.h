// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

/*
 * Balicek funkci pro snadnejsi praci s retezci. Funkce jejichz 
 * nazev konci pismenem A jsou urceny pouze pro vnitrni potreby a 
 * nejsou urceny k pouzivani. Podrobnejsi popis funkci v 
 * souboru ext_string.c.
 */

#ifndef _EXT_STRING_H_
#define _EXT_STRING_H_

/* Kody uspesnosti operaci s retezci. */
#define STR_OK 0 /* Uspech provedeni operace. */
#define STR_ERROR 1 /* Chyba (pri alokaci). */
#define STR_NULL 2 /* Byl predan nealokovany, nebo NULL parametr. */
#define STR_BUFFER 16 /* Velikost bufferu pro aut. alokaci. */

/* Pro spravne chovani funkci nemenit primo slozky teto strutury. */
typedef struct
{
   unsigned int length;
   unsigned int allocated;
   char *str;
} STRING;

int CreateStr(STRING *str, unsigned int bufferSize);
int AssignStrArray(STRING *str, const char *array);
int AssignStrStr(STRING *str, const STRING *src);
int AddStrChar(STRING *str, char addChar);
void RemoveStrChar(STRING *str);
int AppendStr(STRING *dest, const STRING *cat);
int CompareStr(const STRING *s1, const STRING *s2);
int CompareStrArray(STRING *s1, const char *s2);
void FreeStr(STRING *str);

int GetStrLength(const STRING *str);
char *GetStr(const STRING *str);

void InitStringA(STRING *str);
int ResizeStrMemoryA(STRING *str, int newSize);
int AssignStrArrayA(STRING *str, const char *array, 
   unsigned int arrayLength);

#endif
