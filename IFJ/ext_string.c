// Projekt: Implementace imperativniho jazyka IFJ13
// Autori: xbrezi13, Karel Brezina
//         xhavle03, Vojtech Havlena
//         xdobro12, Matus Dobrotka
//         xraszk03, Ales Raszka
//         xjerab18, Kamil Jerabek

#include <stdlib.h>
#include <string.h>
#include "ext_string.h"

/*
 * Funkce, ktera vytvori retezec. 
 * @param str Odkaz na retezec, ktery se ma vytvorit.
 * @param bufferSize Velikost pameti (v bajtech), ktera se ma 
 * vyhradit pro retezec. V pripade 0 nebo zap. cisla se pouzije 
 * konstanta STR_BUFFER.
 * @return Kod uspesnosti vytvoreni retezce.
 */
int CreateStr(STRING *str, unsigned int bufferSize)
{
   if(str == NULL)
      return STR_NULL;
   if(bufferSize == 0)
      bufferSize = STR_BUFFER;
      
   if((str->str = (char *)malloc(sizeof(char) * 
      (bufferSize + 1))) == NULL)
      return STR_ERROR;
   str->allocated = bufferSize + 1;
   str->length = 0;
   
   return STR_OK;
}

/*
 * Funkce pro prirazeni pole znaku do retezce.
 * @param str Retezec, do ktereho se ma priradit.
 * @param array Pole znaku (Cckovsky retezec), ktere se maji priradit 
 * do retezce str.
 * @return Kod uspesnosti prirazeni.
 */
int AssignStrArray(STRING *str, const char *array)
{
   if(str == NULL || array == NULL || str->allocated == 0)
      return STR_NULL;
   return AssignStrArrayA(str, array, strlen(array));
}

/*
 * Funkce pro prirazeni retezce do retezce.
 * @param str Retezec, do ktereho se ma priradit.
 * @param src Retezec, ktery se ma priradit do retezce str.
 * @return Kod uspesnosti prirazeni.
 */
int AssignStrStr(STRING *str, const STRING *src)
{
   if(str == NULL || src == NULL || str->allocated == 0 || 
      src->allocated == 0)
      return STR_NULL;
   return AssignStrArrayA(str, src->str, src->length);
}

/*
 * Funkce zajistujici pridani znaku na konce retezce.
 * @param str Retezec, kam se ma znak pridat.
 * @param addChar Znak pro pridani.
 * @return Kod uspesnosti pridani.
 */
int AddStrChar(STRING *str, char addChar)
{
   if(str == NULL || str->allocated == 0)
      return STR_NULL;
   if(str->length + 1 >= str->allocated)
   {
      if(ResizeStrMemoryA(str, str->allocated + 1) != STR_OK)
         return STR_ERROR;
   }
   str->str[str->length++] = addChar;
   str->str[str->length] = '\0';
   
   return STR_OK;
}

/*
 * Funkce zajistujici pripojeni daneho retezce na konec druheho 
 * retezce (spojeni retezcu).
 * @param dest Cilovy retezec (kam se ma spojeny retezec ulozit).
 * @param cat Retezec, ktery se ma pripojit k retezci dest.
 * @return Kod uspesnosti spojeni.
 */
int AppendStr(STRING *dest, const STRING *cat)
{
   if(dest == NULL || cat == NULL || dest->allocated == 0 || 
      cat->allocated == 0)
      return STR_NULL;
   if(dest->length + cat->length + 1 >= dest->allocated)
   {
      if(ResizeStrMemoryA(dest, dest->length + cat->length + 1) != STR_OK)
         return STR_ERROR;
   }
   strcpy(dest->str + dest->length, cat->str);
   dest->length += cat->length;
   
   return STR_OK;
}

/*
 * Funkce porovnavajici dva retezce.
 * @param s1 1. retezec k porovnani.
 * @param s2 2. retezec k porovnani.
 * @return STR_NULL v pripade chyby, jinak hodnota porovnani.
 */
int CompareStr(const STRING *s1, const STRING *s2)
{
   if(s1 == NULL || s2 == NULL || s1->allocated == 0 || 
      s2->allocated == 0)
      return STR_NULL;
   return strcmp(s1->str, s2->str);
}

/*
 * Funkce porovnavajici retezec s polem znaku.
 * @param s1 Retezec k porovnani.
 * @param s2 Pole znaku, ktere se maji porovnat.
 * @return STR_NULL v pripade chyby, jinak hodnota porovnani.
 */
int CompareStrArray(STRING *s1, const char *s2)
{
   if(s1 == NULL || s2 == NULL || s1->allocated == 0)
      return STR_NULL;
   return strcmp(s1->str, s2);
}

/*
 * Funkce pro uvolneni retezce z pameti.
 * @param str Retezec, ktery se ma uvolnit.
 */
void FreeStr(STRING *str)
{
   if(str == NULL || str->allocated == 0)
   {
      InitStringA(str);
      return;
   }
   free((void *)str->str);
   InitStringA(str);
}

/*
 * Funkce pro zjisteni delky retezce.
 */
int GetStrLength(const STRING *str)
{
   return str->length;
}

/*
 * Funkce pro prevedeni retezce str na pole znaku.
 */
char *GetStr(const STRING *str)
{
   return str->str;
}

/*
 * Vnitrni funkce, ktera nastavi hodnotu retezce na vychozi hodnotu.
 * @param str Retezec, ktery se ma inicializovat.
 */
void InitStringA(STRING *str)
{
   str->length = 0;
   str->allocated = 0;
   str->str = NULL;
}

/*
 * Vnitrni funkce pro realokaci pameti pro ulozeni retezce.
 * @param str Retezec, ktery se ma realokovat.
 * @param newSize Nova velikost pameti (v bajtech).
 * @return Kod uspesnosti.
 */
int ResizeStrMemoryA(STRING *str, int newSize)
{
   if((str->str = (char *)realloc(str->str, 
      sizeof(char) * (newSize + STR_BUFFER))) == NULL)
      return STR_ERROR;
   str->allocated = newSize + STR_BUFFER;
   
   return STR_OK;
}

/*
 * Vnitrni funkce, ktera priradi retezci pole znaku.
 * @param str Retezec, kteremu se ma prirazeni provest.
 * @param array Pole znaku pro prirazeni.
 * @arrayLength Pocet prvku pole.
 * @return Kod uspesnosti.
 */
int AssignStrArrayA(STRING *str, const char *array, unsigned int arrayLength)
{
   if(str->allocated <= arrayLength)
   {
      if(ResizeStrMemoryA(str, arrayLength + 1) != STR_OK)
         return STR_ERROR;
   }
   str->length = arrayLength;
   strcpy(str->str, array);
   
   return STR_OK;
}

/*
 * Funkce pro odstraneni posledniho znaku z retezce.
 * @param str Retezec, jehoz znak se ma odstranit.
 */
void RemoveStrChar(STRING *str)
{
   if(str->length == 0 || str->allocated == 0)
      return;
   str->length--;
   str->str[str->length] = '\0';
}
