
/*
 * Soubor: SyslogHandler.h
 * Datum: 8. 10. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Trida slouzici ke sprave pripojeni a posilani zpravy 
 * na logovaci server. Komunikace probiha pomoci UDP na porte 514.
 */
 
#ifndef _SYSLOG_HANDLER_H_
#define _SYSLOG_HANDLER_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <time.h>
#include <ifaddrs.h>
#include <net/if.h>  

/* Port na kterem komunikujeme s logovacim serverem. */
#define SYSLOG_PORT 514
/* Hodnota polozky hlavicky PRI. */
#define PRI 134

using std::string;

class SyslogHandler
{
private:
   /* Socket pro UDP komunikaci. */
   int sock;
   /* Adresa serveru. */
   sockaddr_in serverAddr;
   
   /* Metody pro generovani zpravy. */
   string GenerateTimestamp() const;
   string GetHost() const;
   string GenerateMessage(const string &nick, const string &message);
   
   /* Zkratky mesicu pro vytvoreni polozky TIMESTAMP ve log. zprave. */
   const char * MONTHS[12] = {
      "Jan", "Feb", "Mar", "Apr", "May", 
      "Jun", "Jul", "Aug", "Sep", "Oct", 
      "Nov", "Dec"
   };
   
public:
   /* Metody pro obsluhu logovani. */
   bool Connect(const char *ipAdress);
   bool SendLog(const string &nick, const string &message);
   void Disconnect() { close(sock); };
};

#endif
