
/*
 * Soubor: Url.h
 * Datum: 1. 3. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Hlavickovy soubor pro tridu umoznujici parsovani 
 * URL retezce v danem formatu.
 */

#ifndef _URL_H_
#define _URL_H_

#include <string>
#include <ctype.h>

class Url
{
   private: 
      std::string url;
   
   public: 
      int port;
      std::string user;
      std::string pass;
      std::string host;
      std::string path;
      bool ftp;
   
   /* Konstruktor */
   Url(std::string & uri)
   {
      this->url = uri;
      this->user = "";
      this->pass = "";
      this->host = "";
      this->path = "";
      this->port = 21;
      this->ftp = false;
   }
   
   bool Parse();
};

#endif
