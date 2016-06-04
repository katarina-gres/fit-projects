
/*
 * tail.c
 * Reseni IJC-DU, priklad a), 20. 4. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 * 
 * Program, ktery ze zadaneho souboru vypise poslednich n radku. 
 * Verze C.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/* Vychozi hodnota poctu vytisknutych radku. */
#define PRINT_LINES 10
/* Maximalni delka radku. */
#define LINE_LONG 1024

#define Count(val,add) ((val) != 0 ? (add) : 0)

/* Makro pro spocitani indexu v bufferu. */
#define BufferIndex(val,max) (((val) >= (max)) ? 0 : (val))

#define Min(a,b) (((a) > (b)) ? (b) : (a))

/* Typ urcujici, zda se ma cist ze souboru nebo ze std. vstupu. */
typedef enum
{
   STDIN_READ,
   FILE_READ
} TSTATES;

/* Chybove kody. */
typedef enum
{
   EOK,
   EPARAMS,
   EFORMAT,
   EFILE,
   EMEMORY,
   ELINE,
   EUNKNOWN
} TERRORS;

/* Struktura pro ulozeni zpracovanych parametru. */
typedef struct
{
   TSTATES state;
   char * fileName;
   unsigned long printLines;
   bool printFrom;
   TERRORS error;
} TPARAMS;

/* Chybove hlasky. */
const char * ERRORMSG[] = {
   "V poradku.",
   "Chybne parametry prikazoveho radku.",
   "Spatny format parametru.",
   "Chyba pri otevirani vstupniho souboru.",
   "Chyba pri alokovani pameti.",
   "Byla prekrocena maximalni delka radku. Budu pokracovat se zkracenymi radky.",
   "Neznama chyba."
};  

/* Prototypy funkci. */
TPARAMS GetInputParams(int argc, char *argv[]);
void PrintError(int code);
FILE * InitInput(char * fileName);
void CloseInput(FILE *intput);
void IgnoreLine(FILE * stream);
void PrintLastLines(char (*buffer)[LINE_LONG], int counter, int n, int max);
int Tail(FILE * inStream, unsigned long lines, bool printFrom);

int main(int argc, char *argv[])
{
   TPARAMS params = GetInputParams(argc, argv);
   if(params.error != EOK)
   {
      PrintError(params.error);
      return EXIT_FAILURE;
   }
   
   FILE * inStream = InitInput(params.fileName);
   if(inStream == NULL)
   {
      PrintError(EFILE);
      return EXIT_FAILURE;
   }
   
   int code = Tail(inStream, params.printLines, params.printFrom);
   CloseInput(inStream);
   if(code != EOK)
   {
      PrintError(code);
      return EXIT_FAILURE;
   }
   
   return EXIT_SUCCESS;
}

/*
 * Funkce, ktera zpracuje parametry programu.
 * @param argc Pocet parametru.
 * @param argv Jednotlive parametry.
 * @return Zpracovane parametry.
 */
TPARAMS GetInputParams(int argc, char *argv[])
{
   TPARAMS ret = { .state = STDIN_READ, .fileName = NULL, 
      .printLines = PRINT_LINES, .printFrom = false, .error = EOK };
   
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
      char convertError = 'Z';
      char *convertPtr = &convertError;
      ret.printLines = strtoul(strTmp, &convertPtr, 10);
      if(*convertPtr != '\0')
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
 * @param inStream Soubor nebo std. vstup ze ktereho se ma cist.
 * @param lines Pocet radek, ktere se maji cist.
 * @param printFrom Ma se cist od n. radku.
 * @return EOK
 */
int Tail(FILE * inStream, unsigned long lines, bool printFrom)
{
   unsigned long printFromLine = lines;
   if(lines == 0 && !printFrom)
      return EOK;
   else if(lines <= 0 && printFrom)
   {
      printFromLine = 1;
   }
   if(printFrom)
      lines = 1;
   char buffer [lines][LINE_LONG];
   unsigned long counter = 0;
   
   char ch = '\0';
   char * found = &ch;
   unsigned long lineCounter = 1;
   bool first = true;
   while(fgets(*(buffer + counter), LINE_LONG, inStream) != NULL)
   {
      if((found = strchr(buffer[counter], '\n')) == NULL && 
         strlen(buffer[counter]) == LINE_LONG - 1)
      {
         if(first)
         {
            PrintError(ELINE);
            first = false;
         }
         IgnoreLine(inStream);
         buffer[counter][LINE_LONG - 2] = '\n';
      }
      
      if(printFrom && printFromLine <= lineCounter)
         printf("%s", buffer[counter]);
      
      counter = BufferIndex(counter + 1, lines);
      lineCounter++;
   }
   
   if(lineCounter - 1 < lines)
      counter = 0;
   if(!printFrom)
      PrintLastLines(buffer, counter, Min(lines, lineCounter - 1), lines);
   
   return EOK;
}

/*
 * Funkce, ktera vytiskne chybu.
 * @param code Kod chyby.
 */
void PrintError(int code)
{
   if(code < EOK || code > EUNKNOWN)
      code = EUNKNOWN;
   fprintf(stderr, "%s\n", ERRORMSG[code]);
}

/*
 * Funkce, ktera inicializuje vstup, ze ktereho se bude cist.
 * @param fileName Jmeno souboru.
 * @return Inicializovany vstup.
 */
FILE * InitInput(char * fileName)
{
   if(fileName == NULL)
   {
      return stdin;
   }
   else
   {
      return fopen(fileName, "r"); 
   }
}

/*
 * Funkce, ktera uzavre vstupni soubor.
 * @param intput Soubor, ktery se ma uzavrit.
 */
void CloseInput(FILE *intput)
{
   if(intput != stdin && intput != NULL)
      fclose(intput);
}

/*
 * Funkce, ktera ignoruje zbytek radku.
 * @param stream Vstup, ze ktereho se ma cist.
 */
void IgnoreLine(FILE * stream)
{
   int actChar = 0;
   while((actChar = fgetc(stream)) != EOF && actChar != '\n')
   { }
}

/*
 * Funkce, ktera vytiskne obsah bufferu.
 * @param buffer Buffer, ktery se ma vypsat.
 * @param counter Pocatecni index v buffer.
 * @param n Obsazenost bufferu.
 * @param max Velikost bufferu.
 */
void PrintLastLines(char (*buffer)[LINE_LONG], int counter, int n, int max)
{
   for(int i = 0; i < n; i++)
   {
      printf("%s", buffer[counter]);
      counter = BufferIndex(counter + 1, max);
   }
}
