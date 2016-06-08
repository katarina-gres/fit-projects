
/*
 * Soubor: lngTerminal.h
 * Autor: Vojtech Havlena (xhavle03), Karel Brezina (xbrezi13)
 * 
 * Hlavickovy soubor k tride reprezentujici LNG terminal.
 */

#ifndef _LNG_TERMINAL_H_
#define _LNG_TERMINAL_H_

#include <string.h>
#include "gasPipeline.h"

using std::string;

class GasPipeline;

/* Trida, ktera reprezentuje LNG terminal. */
class LngTerminal
{
private:
   /* Produkce */
   long production;
   /* Kapacita */
   long capacity;
   /* Aktualni mnozstvi ulozeneho plynu */
   long actStorage;
   
   /* Jmeno terminalu */
   string name;
   
public:
   /* 
    * Konstruktor.
    * @param na Jmeno terminalu.
    * @param prod Produkce
    * @param cap Kapacita terminalu. 
    */
   LngTerminal(string na, long prod, long cap) : production(prod), 
      capacity(cap), name(na), actStorage(production) { }
   void ProcessHour(long time);
   void CheckStatus();
   
   /*
    * Metoda, ktera zjistuje kapacitu terminalu.
    * @return Kapacita terminalu.
    */
   long GetCapacity() const { return capacity; }
   /*
    * Metoda, ktera nastavuje kapacitu terminalu.
    * @param cap Kapacita terminalu.
    */
   void SetCapacity(long cap) { capacity = cap; production = cap; }
   
   /*
    * Plynovody, kudy se ma posilat plyn.
    */
   map<long, GasPipeline*> sendStates;
};

#endif
