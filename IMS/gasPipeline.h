
/*
 * Soubor: gasPipeline.h
 * Autor: Vojtech Havlena (xhavle03), Karel Brezina (xbrezi13)
 * 
 * Hlavickovy soubor k tride reprezentujici plynovod.
 */

#ifndef _GAS_PIPELINE_H_
#define _GAS_PIPELINE_H_

#include <queue>   
#include <vector>   
#include <functional>
#include "state.h"

using namespace std;

/* Rychlost plynu */
#define GASSPEED 40 // km/h

class State;

/* Polozka kalendare. */
struct CalItem
{
   long time; /* Cas doruceni. */
   long gas; /* Mnozstvi dorucovaneho plynu. */
   State * to; /* Komu se ma plyn dorucit. */
};

/* Porovnavani pro ulozeni do prioritni frony podle data. */
struct CompareTime 
{
    bool operator()(CalItem const & p1, CalItem const & p2) 
    {
        return p1.time > p2.time;
    }
};

/* Datovy typ pro kalendar. */
typedef priority_queue<CalItem, vector<CalItem>, CompareTime> CALENDAR;

/* Trida, ktera reprezentuje plynovod. */
class GasPipeline
{
private:
   /* Kapacita plynovodu. */
   long capacity;
   /* Od koho je plyn dorucen. */
   State * from;
   /* Cilovy stat. */
   State * to;
   /* Mnozstvi posilaneho plynu. */
   long send;
   /* Delka plynovodu. */
   long length;
   
   /* Statistika -- prumerne vytizeni. */
   int n = 0;
   double average = 0.0;
   
   /* Kalendar. */
   CALENDAR *calendar;
   
public:
   /* 
    * Konstruktor.
    * @param fr Zdrojovy stat.
    * @param t Cilovy stat.
    * @param cap Kapacita plynovodu.
    * @param len Delka plynovodu.
    * @param cal Kalendar. 
    */
   GasPipeline(State *fr, State *t, long cap, long len, CALENDAR *cal) : from(fr), 
      to(t), capacity(cap), calendar(cal), length(len), send(0) { }
   /*
    * Metoda, ktera prida mnozstvi plynu, ktere se ma plynovodem poslat.
    * @param gas Mnozstvi odesilaneho plynu.
    */
   void AddSendGas(long gas) { send += gas; }
   /*
    * Metoda, ktera posle plyn.
    * @param time Aktualni cas.
    */
   void SendGas(long time);
   
   /*
    * Metody pro zjisteni zdroje a cile plynovodu.
    */
   State * From() { return from; }
   State * To() { return to; }
   
   /*
    * Metoda, ktera zjistuje kapacitu plynovodu.
    * @return Kapacita plynovodu.
    */
   long GetCapacity() const { return capacity; }
   /*
    * Metoda, ktera zjistuje prumerne vytizeni plynovodu.
    * @return Prumerne vytizeni plynovodu.
    */
   double AverageUtil() { return average; }
};

#endif
