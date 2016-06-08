
/*
 * Soubor: state.h
 * Autor: Vojtech Havlena (xhavle03), Karel Brezina (xbrezi13)
 * 
 * Hlavickovy soubor k tride reprezentujici dany stat.
 */

#ifndef _STATE_H_
#define _STATE_H_

#include "gasPipeline.h"

#include <string.h>
#include <map>
#include <iostream>
#include <stdlib.h>   

using std::map;
using std::string;

using std::cout;
using std::endl;

/* Odchylka od prumerne produkce a spotreby. */
#define PRODUCTION 10 //plus minus %
#define CONSUMPTION 10

/* Strategie pro export plynu. */
enum STRATEGY
{
   CONTRACT,
   GREEDY
};

class GasPipeline;

/* Trida, ktera reprezentuje stat. */
class State
{
private:
   /* Spotreba */
   long consumption;
   /* Produkce */
   long production;
   /* Kapacita zasobniku */
   long capacity;
   
   /* Import od neuvazovanych statu. */
   long importOther = 0;
   
   /* Mnozstvi ulozeneho plynu. */
   long stored;
   string name;
   
   /* Mnozstvi importovaneho plynu od ostatnich statu. */
   long importGas = 0;
   long importLast = 0;
   
   /* Letni spotreba? */
   bool summerConsumption = false;
   /* Koeficient letni spotreby. */
   double summerRatio;
   
   /* Statistika -- prumerna hodnota zasobniku. */
   long n = 0;
   double average = 0.0;
   
   /* Pres jake plynovody ma probihat export. */
   map<long, GasPipeline*> * sendStates = NULL;
   /* Celkova vystupni kapacita. */
   long sumOutput = 0;
   
public:
   /*
    * Konstruktor.
    * @param nam Jmeno statu
    * @param cons Spotreba
    * @param prod Produkce
    * @param cap Kapacita zasobniku
    * @param sRatio Koeficient letni spotreby.
    */
   State(string nam, long cons, long prod, long cap, double sRatio) : name(nam), 
      consumption(cons), production(prod), capacity(cap), stored(0.9*cap), 
      summerRatio(sRatio) { }
   
   /*
    * Nastaveni importovaneho plynu.
    * @param val Mnozstvi importovaneho plynu.
    */
   void SetImportVal(long val) { importGas = val; }
   /*
    * Metoda, ktera vraci mnozstvi importovaneho plynu.
    * @return Mnozstvi importovaneho plynu.
    */
   long GetImportVal() const { return importGas; }
   /*
    * Metoda, ktera vraci mnozstvi ulozeneho plynu.
    * @return Mnozstvi ulozeneho plynu.
    */
   long GetStored() { return stored; }
   /*
    * Nastaveni mnozstvi ulozeneho plynu.
    * @param val Mnozstvi ulozeneho plynu.
    */
   void SetStored(long val) { stored = val; }
   /*
    * Jmeno statu.
    */
   string Name() { return name; }
   /*
    * Nastaveni letni spotreby.
    * @param val Je letni spotreba?
    */
   void SetSummerConsumption(bool cons) { summerConsumption = cons; }
   /*
    * Metoda, ktera vraci informaci o tom, jestli je letni spotreba.
    * @return Letni spotreba.
    */
   bool GetSummerConsumption() const { return summerConsumption; }
   
   void ProcessHour();
   void CheckStatus();
   void SendGas(STRATEGY strategy, long time);
   
   /*  
    * Prumerna spotreba.
    */
   double AverageUtil() { return average; }
   void ClearAverage() { n = 0; average = 0.0; }
   
   
   void SetSendStates(map<long, GasPipeline*> * send);
   /*
    * Nastaveni mnozstvi importovaneho plynu od neuvazovanych statu.
    */
   void SetImportOther(long im) { importOther = im; }
};



#endif
