
/*
 * Soubor: SyslogHandler.cpp
 * Datum: 8. 10. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Zdrojovy soubor implementujici metody tridy SyslogHandler, ulozeny v 
 * souboru SyslogHandler.h.
 */

#include "SyslogHandler.h"

/*
 * Metoda, ktera se pripoji k logovacimu serveru.
 * @param ipAddress IP adresa logovaciho serveru.
 * @return Uspesnost pripojeni.
 */
bool SyslogHandler::Connect(const char *ipAddress)
{
   in_addr ipAddr;
   
   if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
   { 
      return false; 
   }
   
   inet_pton(AF_INET, ipAddress, &ipAddr);
   
   memset((char*)&serverAddr, 0, sizeof(serverAddr)); 
   serverAddr.sin_family = AF_INET; 
   serverAddr.sin_port = htons(SYSLOG_PORT);
   memcpy((void *)&serverAddr.sin_addr, &ipAddr, sizeof(ipAddr));
   
   return true;
}

/*
 * Metoda, ktera posle zpravu na logovaci server.
 * @param nick Nick odesilatele zpravy (ulozi se do polozky TAG).
 * @param message Obsah zpravy, ktera byla dorucena pres IRC botovi.
 * @return Uspesnost odeslani zpravy.
 */
bool SyslogHandler::SendLog(const string &nick, const string &message)
{
   /* Vytvoreni cele zpravy i s hlavickou. */
   string complet = GenerateMessage(nick, message);

   if(sendto(sock, complet.c_str(), complet.length(), 0, 
      (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) 
   {
      return false;
   }
   
   return true;
}

/*
 * Metoda, ktera vygeneruje polozku zpravy TIMESTAMP v souladu s RFC.
 * @return Vygenerovana polozka TIMESTAMP.
 */
string SyslogHandler::GenerateTimestamp() const
{
   char buff[20];
   time_t timeHandler = time(NULL);
   tm *actTime = localtime(&timeHandler);
   
   sprintf(buff, "%s %2d %.2d:%.2d:%.2d", MONTHS[actTime->tm_mon], 
      actTime->tm_mday, actTime->tm_hour, actTime->tm_min, actTime->tm_sec);
      
   return string(buff);
}

/*
 * Metoda, ktera vygeneruje kompletni zpravu, ktera se pouzije pro 
 * odeslani na logovaci server.
 * @param nick Nick odesilatele zpravy (ulozi se do polozky TAG).
 * @param message Obsah zpravy, ktera byla dorucena pres IRC botovi.
 * @return Vygenerovana kompletni zprava.
 */
string SyslogHandler::GenerateMessage(const string &nick, const string &message)
{
   string ret = "<134>" + GenerateTimestamp() + " " + GetHost();
   if(nick == "")
   {
      ret += " ircbot: " + message;
   }
   else
   {
      ret += " ircbot <" + nick + ">: " + message;
   }
   
   return ret;
}

/*
 * Metoda, zjistujici lokalni IP adresu. Vzhledem k predpokladu 
 * existence pouze 1 rozhrani, pouzije se IP adresa prvniho nalezeneho 
 * (vyjma loopback).
 * @return Lokalni IP adresa.
 */
string SyslogHandler::GetHost() const
{
   void *addrPointer = NULL;
   char buff[INET_ADDRSTRLEN];
   
   ifaddrs *ipAddress = NULL;
   ifaddrs *iter = NULL;
   
   getifaddrs(&ipAddress);
   if(ipAddress == NULL)
      return "";

   iter = ipAddress;
   for(;iter != NULL; iter = iter->ifa_next) 
   {
      if (iter->ifa_addr == NULL)
         continue;
      /* Jedna se o loopback. */
      if(iter->ifa_flags & IFF_LOOPBACK)
         continue;
        
      if (iter->ifa_addr->sa_family == AF_INET) 
      {
         addrPointer = &((sockaddr_in *)iter->ifa_addr)->sin_addr;
         inet_ntop(AF_INET, addrPointer, buff, INET_ADDRSTRLEN);
         
         break;
      }
   }
   freeifaddrs(ipAddress);
   
   return string(buff);
}
