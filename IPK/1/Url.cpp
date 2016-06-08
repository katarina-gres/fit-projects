
/*
 * Soubor: Url.cc
 * Datum: 1. 3. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Zdrovovy soubor pro tridu parsujici URL retezec.
 */

#include "Url.h"

/*
 * Metoda, ktera zpracuje URL retezec a rozdeli ho do pozadovanych 
 * casti (jmeno, heslo, server ...).
 * @return Uspesnost parsovani.
 */
bool Url::Parse()
{
   int i = 0;
   bool usr = false, portDef = false;
   int length = this->url.size();
   if(this->url.substr(0, 6) == "ftp://")
   {
      ftp = true;
      i = 6;
   }
   
   if(this->url.find("@") != std::string::npos)
      usr = true;
      
   if(usr)
   {
      while(this->url[i] != '@')
      {
         if(this->url[i] == ':') 
         {
            usr = false;
            i++;
            continue;
         }
         if(usr) 
            this->user.push_back(this->url[i]);
         else
            this->pass.push_back(this->url[i]);
         i++;
      }
      i++;
   }

   while(i < length)
   {
      if(this->url[i] == ':')
      {
         portDef = true;
         i++;
         break;
      }
      else if(this->url[i] == '/')
      {
         i++;
         break;
      }
      this->host.push_back(this->url[i]);
      i++;
   }
   
   if(portDef)
   {
      this->port = 0;
      while(isdigit(this->url[i]))
      {
         this->port *= 10;
         this->port += (this->url[i] - '0');
         i++;
      }
      if(i < length && this->url[i] != '/')
         return false;
   }
   for(; i < length; i++)
      this->path.push_back(this->url[i]);
   
   return true;
}
