/**
 * @file client.cc
 * @author Vojtech Havlena <xhavle03@stud.fit.vutbr.cz>
 * @date 10. 4. 2014
 * 
 * @section DESCRIPTION
 * Soubor implementace klienta pro posilani souboru s korigovanim
 * rychlosti prenostu.
 * 
 * IPK projekt 2.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

/**
 * Struktura pro parametry prikazoveho radku.
 * Host pro pripojeni, posilany soubor a port pro prenos.
 */
struct Params
{
   string host;
   string file;
   int port;
};

/**
 * Mozne chybove kody.
 */
enum ERRORS {
   EOK,
   ESOCKET,
   ECONNECT,
   EWRITE,
   EFILE,
   EACCEPT,
   EPARAMS,
   EMESSFORMAT
};

/**
 * Jednotlive chybove hlasky (vztahujici se k chybovym kodum).
 */
string ERROR_MSG[] = {
   "V poradku.", "Chyba pri vytvareni/ruseni socketu.",
   "Chyba pri navazovani spojeni.",
   "Chyba pri zasilani pozadavku.", 
   "Chyba pri otevirani souboru.",
   "Chyba pri prijimani dat.",
   "Chybne parametry.",
   "Chybny format zasilane zpravy."
};

bool ParseArgs(int argc, char *argv[], Params *ret);
void PrintError(int code);

/**
 * Hlavni funkce main.
 */
int main (int argc, char *argv[] )
{
   /**
    * Zpracovani parametru.
    */
   Params params;
   if(!ParseArgs(argc, argv, &params))
   {
      PrintError(EPARAMS);
      return EPARAMS;
   }
   if(params.host == "" || params.file == "")
   {
      PrintError(EPARAMS);
      return EPARAMS;
   }
   
   int sock;
   sockaddr_in sin; 
   hostent *host;
   string message = "FILE " + params.file + '\n';

   /**
    * Vytvoreni socketu a spojeni se se serverem.
    */
   if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
   {
      PrintError(ESOCKET);
      return ESOCKET;
   }
   sin.sin_family = PF_INET;
   sin.sin_port = htons(params.port);
   if ((host = gethostbyname(params.host.c_str())) == NULL)
   {
      PrintError(ECONNECT);
      return ECONNECT;
   }
   
   memcpy( &sin.sin_addr, host->h_addr, host->h_length);
   if (connect(sock,(struct sockaddr *)&sin, sizeof(sin)) < 0)
   {
      PrintError(ECONNECT);
      return ECONNECT;
   }
   /**
    * Posleme pozadavek o soubor na server.
    */
   if (write(sock, message.c_str(), message.size()) < 0) 
   {  
      PrintError(EWRITE);
      return EWRITE;
   }
   
   long total = 0;
   long ackLength = 0;
   string readMsg = "";
   char buff;
   bool retMsg = false;
   /**
    * Otevreme soubor pro zapis pro ulozeni poslaneho souboru.
    */
   ofstream outFile(params.file, ios::binary|ios::out);
   if(!outFile.is_open())
   {
      PrintError(EFILE);
      return EFILE;
   }
   
   /**
    * Cteni zpravy poslaneho serverem. Prvni radek je informace
    * o uspesnosti otevreni souboru a pocet bytu souboru.
    */
   while(read(sock, &buff, 1) > 0)
   {
      if(!retMsg)
      {
         if(buff == '\n')
         {
            if(readMsg.size() < 6)
            {
               PrintError(EACCEPT);
               return EACCEPT;
            }
            if(readMsg.substr(0, 4) != "EOK:")
            {
               PrintError(EACCEPT);
               return EACCEPT;
            }
            //ackLength = stol(readMsg.substr(4));
				ackLength = atol(readMsg.substr(4).c_str());
            
            readMsg = "";
            retMsg = true;
         }
         else
         {
            readMsg += buff;
         }
         continue;
      }
      readMsg += buff;
      
      outFile.write(readMsg.c_str(), 1);
      readMsg = "";
      total++;
   }
   
   outFile.close();
   if(ackLength != total)
   {
      PrintError(EACCEPT);
      return EACCEPT;
   }

   if (close(sock) < 0) { 
      PrintError(ESOCKET);
      return ESOCKET;
   }
   return 0;
}

/**
 * Funkce pro parsovani argumentu prikazoveho radku.
 * @param argc Pocet argumentu.
 * @param argv Jednotlive parametry.
 * @param ret Vystupni parametr pro vraceni nactenych hodnot.
 * @return Uspesnost parsovani.
 */
bool ParseArgs(int argc, char *argv[], Params *ret)
{
   if(argc != 2)
   {
      return false;
   }
      
   int pos = 0;
   
   ret->host = "";
   ret->file = "";
   ret->port = -1;
   while(argv[1][pos] != '\0' && argv[1][pos] != ':')
   {
      ret->host += argv[1][pos];
      pos++;
   }
   pos++;
   
   char *endPtr;
   ret->port = strtol(argv[1] + pos, &endPtr, 10);
   if(endPtr == argv[1] + pos)
   {
      return false;
   }
   if(*endPtr != '/')
   {
      return false;
   }
   
   endPtr++;
   while(*endPtr != '\0')
   {
      ret->file += *endPtr;
      endPtr++;
   }
   
   return true;
}

/**
 * Funkce, ktera vytiskne chybovou hlasku podle chyboveho kodu.
 * @param code Kod chyby.
 */
void PrintError(int code)
{
   cerr << ERROR_MSG[code] << endl;
}
