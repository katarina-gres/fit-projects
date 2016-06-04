
/*
 * tail2.cc
 * Reseni IJC-DU, priklad a), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: G++ 4.7
 * 
 * Program, ktery ze zadaneho souboru vypise poslednich n radku. 
 * Verze C++.
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <deque>
#include <sstream>

/* Vychozi hodnota poctu vytisknutych radku. */
const unsigned int PRINT_LINES =  10;

inline int Count(int val, int add)
{
   return (val != 0) ? add : 0;
}

/* Typ urcujici, zda se ma cist ze souboru nebo ze std. vstupu. */
enum TSTATES
{
   STDIN_READ,
   FILE_READ
};

/* Chybove kody. */
enum TERRORS
{
   EOK,
   EPARAMS,
   EFORMAT,
   EFILE,
   EMEMORY,
   EUNKNOWN
};

/* Struktura pro ulozeni zpracovanych parametru. */
struct TPARAMS
{
   TSTATES state;
   char * fileName;
   unsigned long printLines;
   bool printFrom;
   TERRORS error;
};

/* Chybove hlasky. */
const char * ERRORMSG[] = {
   "V poradku.",
   "Chybne parametry prikazoveho radku.",
   "Spatny format parametru.",
   "Chyba pri otevirani vstupniho souboru.",
   "Chyba pri alokovani pameti.",
   "Neznama chyba."
};  

using namespace std;

/* Prototypy funkci. */
TPARAMS GetInputParams(int argc, char *argv[]);
void PrintError(int code);
void PrintLastLines(deque<string> & deck);
void Tail(istream * stream, unsigned long lines, bool printFrom);

int main(int argc, char *argv[])
{
   ios::sync_with_stdio(false);
   TPARAMS params = GetInputParams(argc, argv);
   if(params.error != EOK)
   {
      PrintError(params.error);
      return 1;
   }
   
   istream * stream = &cin;
   fstream fileStream;
   if(params.fileName != NULL)
   {
      fileStream.open(params.fileName);
      if(!fileStream.is_open())
      {
         PrintError(EFILE);
         return 1;
      }
      stream = &fileStream;
   }
   
   Tail(stream, params.printLines, params.printFrom);
   fileStream.close();
   
   return 0;
}

/*
 * Funkce, ktera zpracuje parametry programu.
 * @param argc Pocet parametru.
 * @param argv Jednotlive parametry.
 * @return Zpracovane parametry.
 */
TPARAMS GetInputParams(int argc, char *argv[])
{
   TPARAMS ret = { STDIN_READ, NULL, 
      PRINT_LINES, false, EOK };
   
   int nIndex = 0, fileIndex = 0;
   for(int i = 1; i < argc; i++)
   {
      if(nIndex == 0 && strcmp(argv[i], "-n") == 0)
      {
         nIndex = i;
      }
   }
   fileIndex = (nIndex == 1) ? nIndex + 2 : nIndex - 1;
   if(nIndex == 0)
      fileIndex = 1;
   
   if(argc > 2 + Count(nIndex, 2))
   {
      ret.error = EPARAMS;
      return ret;
   }
   if(fileIndex >= 1 && fileIndex < argc)
   {
      ret.fileName = argv[fileIndex];
      ret.state = FILE_READ;
   }
   if(nIndex != 0)
   {
      if(nIndex + 1 >= argc)
      {
         ret.error = EFORMAT;
         return ret;
      }
      char *strTmp = argv[nIndex + 1];
      if(strTmp[0] == '+')
      {
         strTmp++;
         ret.printFrom = true;
      }
      if(!isdigit(strTmp[0]))
      {
         ret.error = EFORMAT;
         return ret;
      }
      
      stringstream str(strTmp);
      str >> ret.printLines;
      if(!str)
      {
         ret.error = EFORMAT;
         return ret;
      }
   }
   
   return ret;
}

/*
 * Funkce, ktera vypisuje poslednich n radku (nebo vypisuje od n. radku)
 * zadaneho souboru.
 * @param stream Soubor nebo std. vstup ze ktereho se ma cist.
 * @param lines Pocet radek, ktere se maji cist.
 * @param printFrom Ma se cist od n. radku.
 * @return EOK
 */
void Tail(istream * stream, unsigned long lines, bool printFrom)
{
   unsigned long printFromLine = lines;
   if(printFrom)
      lines = 1;
   
   unsigned long lineCounter = 1;
   string line;
   deque<string> deck;
   
   while(getline(*stream, line))
   {
      if(printFrom && printFromLine <= lineCounter)
         cout << line << endl;
         
      deck.push_back(line);
      if(deck.size() > lines)
         deck.pop_front();
      
      lineCounter++;
   }
   
   if(!printFrom)
      PrintLastLines(deck);
}

/*
 * Funkce, ktera vytiskne chybu.
 * @param code Kod chyby.
 */
void PrintError(int code)
{
   if(code < EOK || code > EUNKNOWN)
      code = EUNKNOWN;
   cerr << ERRORMSG[code] << endl;
}

/*
 * Funkce, ktera vytiskne obsah fronty.
 * @param deck Fronta k vytisknuti.
 */
void PrintLastLines(deque<string> & deck)
{
   for(deque<string>::iterator it = deck.begin(); it != deck.end(); it++)
   {
      cout << *it << endl;
   }
}
