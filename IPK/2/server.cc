/**
 * @file server.cc
 * @author Vojtech Havlena <xhavle03@stud.fit.vutbr.cz>
 * @date 10. 4. 2014
 * 
 * @section DESCRIPTION
 * Soubor implementace serveru pro posilani souboru s korigovanim
 * rychlosti prenosu.
 * 
 * IPK projekt 2
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
#include <sstream>
#include <thread>
#include <ctime>

using namespace std;

/** Velikost bloku cteneho ze souboru. */
#define BUFF_SIZE 1000
/** Velikost odesilaneho bloku dat. */
#define BLOCK_SIZE 1000

/** 
 * Chybove kody. 
 */
enum ERRORS {
   EOK,
   ESOCKET,
   EBIND,
   ELISTEN,
   ECONN,
   EWRITE,
   EFILE,
   EPARAMS,
   EMESSFORMAT
};

/** 
 * Chybove hlasky vztahujici se k jednotlivym chybovym kodum. 
 */
string ERROR_MSG[] = {
   "V poradku.", "Chyba pri vytvareni/ruseni socketu.",
   "Chyba pri bind.", "Chyba pri naslouchani pri spojeni.",
   "Chyba pri navazovani spojeni.", 
   "Chyba pri zasilani dat.",
   "Chyba pri otevirani souboru.",
   "Chybne parametry.",
   "Chybny format zasilane zpravy."
};

/** 
 * Prototypy funkci. 
 */
void ParallelTask(int socket, int limit);
void PrintError(int code);
void SendError(int code, int socket);

/**
 * Hlavni funkce main.
 */
int main (int argc, char *argv[])
{
   int s, t, sinlen;
   struct sockaddr_in sin;
   
   /**
    * Promenne pro getopt.
    */
   int c;
   char *limitValue = NULL;
   char *portValue = NULL;
   int limit, port;
   bool setLimit = false, setPort = false;
   /** 
    * Nacteni parametru.
    */
   while ((c = getopt (argc, argv, "d:p:")) != -1)
   {
      switch (c)
      {
         case 'd':
            limitValue = optarg;
            setLimit = true;
            break;
         case 'p':
            portValue = optarg;
            setPort = true;
            break;
         case '?':
            PrintError(EPARAMS);
            return EPARAMS;
         default:
            PrintError(EPARAMS);
            return EPARAMS;
      }
   }
   
   if(!setLimit || !setPort)
   {
      PrintError(EPARAMS);
      return EPARAMS;
   }
   
   /**
    * Prevedeni parametru na cisla + kontrola pripadnych
    * formatovych chyb.
    */
   char *end;
   limit = strtoul(limitValue, &end, 10);
   if(*end != '\0')
   {
      PrintError(EPARAMS);
      return EPARAMS;
   } 
   port = strtoul(portValue, &end, 10);
   if(*end != '\0')
   {
      PrintError(EPARAMS);
      return EPARAMS;
   } 

   /**
    * Vytvoreni socketu a jeho navazani.
    */
   if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0) 
   {
      PrintError(ESOCKET);
      return ESOCKET;
   }
   sin.sin_family = PF_INET;
   sin.sin_port = htons(port);
   sin.sin_addr.s_addr  = INADDR_ANY;
   if (bind(s, (sockaddr *)&sin, sizeof(sin) ) < 0 ) 
   {
      PrintError(EBIND);
      return EBIND;
   }
   if (listen(s, 5)) 
   { 
      PrintError(ELISTEN);
      return ELISTEN;
   }
   sinlen = sizeof(sin);
  
   while (1) 
   {
      /**
       * Pripojeni klienta.
       */
      if((t = accept(s, (struct sockaddr *) &sin, (socklen_t *)&sinlen) ) < 0) 
      {
         PrintError(ECONN);
         return ECONN;
      } 
      
      /**
       * Vytvoreni vlakna pro zpracovavani infa od klientu.
       */
      thread sendThread(ParallelTask, t, limit);
      sendThread.detach();
   }
   if (close(s) < 0) 
   { 
      PrintError(ESOCKET);
      return ESOCKET;
   }
   
   return 0;
}

/**
 * Funkce, pro paralelni zpracovavani pozadavku od klientu.
 * @param socket Socket pouzity pro cteni a zapis.
 * @param limit Rychlostni limit.
 */
void ParallelTask(int socket, int limit)
{
   char buffer[limit*1000];
   ifstream sendFile;
   streampos begin, end;
   
   string message = "";
   char input;

   /**
    * Nacteni zpravy od klienta.
    */
   while(read(socket, &input, 1) == 1)
   {
      if(input == '\n')
         break;
      message.append(&input, 1);
   }
   
   /**
    * Otevreni souboru zadaneho ve zprave.
    */
   string file;
   if(message.substr(0, 4) == "FILE")
   {
      file = message.substr(5);
      sendFile.open(file.c_str(), ios::binary);
      if(!sendFile.is_open())
      {
         PrintError(EFILE);
         SendError(EFILE, socket);
         close(socket);
         return;
      }
   }
   else
   {
      PrintError(EMESSFORMAT);
      SendError(EMESSFORMAT, socket);
      close(socket);
      return;
   }
   
   /**
    * Spocteni velikosti posilaneho souboru.
    */
   begin = sendFile.tellg();
   sendFile.seekg (0, ios::end);
   end = sendFile.tellg();
   unsigned long size = end - begin;
   
   /**
    * Posleme zpravu o velikosti souboru a ze se podarilo soubor
    * otevrit.
    */
   ostringstream stm;
	stm << size;
   string okMsg = "EOK:" + stm.str() + '\n';
   if (write(socket, okMsg.c_str(), okMsg.size()) < 0) 
   {
      PrintError(EWRITE);
      sendFile.close();
      close(socket);
      return;
   }
   
   /**
    * Postupne nacitani obsahu souboru a posilani jej klientovi.
    */
   sendFile.seekg (0, ios::beg);
   unsigned long blockSize = limit * 1000;
   chrono::time_point<chrono::system_clock> start, stop;
   chrono::duration<double> elapsed;
   
   while(!sendFile.eof())
   {
      sendFile.read(buffer, blockSize);
      start = chrono::system_clock::now();
      if (write(socket, buffer, sendFile.gcount()) < 0) 
      {
         PrintError(EWRITE);
         sendFile.close();
         close(socket);
         return;
      }
      stop = chrono::system_clock::now();
      elapsed = stop - start;
      
      //this_thread::sleep_for(chrono::microseconds(wait));
      if(elapsed.count() <= 1.0)
      {
         this_thread::sleep_for(chrono::duration<double>(1.0 * 
            (double)sendFile.gcount() / blockSize) - elapsed);
      }
   }

   sendFile.close();

   if (close(socket) < 0) 
   { 
      PrintError(ESOCKET);
      return; 
   } 
}

/**
 * Funkce, ktera vytiskne chybu na chybovy vystup.
 * @param code Chybovy kod.
 */
void PrintError(int code)
{
   cerr << ERROR_MSG[code] << endl;
}

/**
 * Funkce, ktera odesle chybovou zpravu klientovi.
 * @param code Chybovy kod.
 * @param socket Socket na ktery se ma zprava poslat.
 */
void SendError(int code, int socket)
{
   string msg = "BAD: " + ERROR_MSG[code] + '\n';
   write(socket, msg.c_str(), msg.size());
}
