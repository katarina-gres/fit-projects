
/*
 * Soubor: IrcParser.h
 * Datum: 1. 10. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Trida slouzici k parsovani IRC zprav prichozich ze serveru. 
 * Implementovano podle gramatiky v RFC 1459.
 */

#ifndef _IRCPARSER_H_
#define _IRCPARSER_H_

#include <string>
#include <ctype.h>

/* Konstanta pro konec vstupu. */
#define END -1

using std::string;

/* Jednotlive stavy. */
enum STATE
{
   INFO,
   MESSAGE,
   ERROR
};

/*
 * Trida pro parsovani IRC zprav.
 */
class IrcParser
{
private: 
   STATE state;
   /* Parametry prikazu. */
   string value;
   /* Prikaz (nebo cislo chyby). */
   string command;
   /* Prefix zpravy. */
   string prefix;
   
   /* Radek (zprava), ktera se ma zpracovat. */
   string line;
   /* Index v radku. */
   unsigned int lineIndex;
   
   int GetNextChar();
   /*
    * Metoda, ktera vraci aktualni znak ctene zpravy.
    * @return Aktualni znak.
    */
   int GetActChar() const { return line[lineIndex - 1]; }
   
public:
   /*
    * Metoda, ktera vraci stav.
    * @return Stav.
    */
   STATE GetState() const { return state; }
   /*
    * Metoda, ktera vraci zpracovane parametry prikazu.
    * @return Parametry prikazu.
    */
   string GetValue() const { return value; }
   /*
    * Metoda, ktera vraci nazev zpracovaneho prikazu.
    * @return Zpracovany prikaz.
    */
   string GetCommand() const { return command; }
   /*
    * Metoda, ktera vraci prefix zpravy.
    * @return Zpracovany prefix.
    */
   string GetPrefix() const { return prefix; }
   
   static string GetNickFromPrefix(string prefix);
   static string GetMessageFromParams(string params);
   
   void ProcessLine(const char * line);
   
protected:
   bool Message();
   bool Prefix();
   bool Command();
   bool Params();

};

#endif
