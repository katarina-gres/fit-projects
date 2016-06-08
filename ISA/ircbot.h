
/*
 * Soubor: ircbot.h
 * Datum: 1. 10. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Hlavickovy soubor pro samotny program ircbot. Obsahuje konstanty a
 * chybove hlasky.
 */

#ifndef _IRCBOT_H_
#define _IRCBOT_H_

#include <vector>
#include <string>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <netinet/tcp.h>

#include <fcntl.h>


#include "SyslogHandler.h"

/* Pocet prikazu potrebnych pro navazani spojeni s IRC. */
#define COMMANDS 3

/* Jednotlive kody chyb. */
enum ERRORS {
   EOK,
   ECONNECT,
   EHOST,
   ESOCKET,
   ESEND,
   EPARAMS,
   ESYSLOG,
   ELOGMESS
};

/* Jednotlive chybove zpravy odpovidajici chybovym kodum. */
string ERROR_MSG[] = {
   "V poradku.", "Chyba pri navazovani spojeni s IRC serverem.",
   "Zadana neplatna adresa IRC serveru.", "Chyba pri vytvareni soketu.",
   "Chyba pri odesilani dat na IRC server.", 
   "Spatne parametry programu.",
   "Nepodarilo se pripojit k logovacimu serveru.",
   "Chyba pri zasilani logovaci zpravy."
};

/* Prikazy posilane IRC serveru. */
string commands[] = 
{
   "NICK xhavle03\r\n",
   "USER xhavle03 xhavle03 xhavle03 xhavle03\r\n",
   "JOIN ",
   "QUIT\r\n"
};

/* Struktura pro nactene parametry programu. */
typedef struct
{
   char *host; /* IRC host. */
   char *channel; /* Pouzity kanal. */
   char *syslogServer; /* Syslog server. */

   std::vector<string> words; /* Jednotliva slova. */
   int port; /* Port pro pripojeni k IRC. */
} PARAMS;

/* Prototypy funkci. */
bool ParseParams(int argc, char *argv[], PARAMS *params);
void PrintError(ERRORS code, bool log);
bool ContainsMessageWord(string &mess, std::vector<string> &words);
bool IsError(string &command);
void CloseConnection();
string ParseError(string &error);

bool ReadIrcMessages(int sock, PARAMS *params, SyslogHandler *handler);
void SignalHandler(int signo);

#endif
