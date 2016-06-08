
/*
 * Soubor: IrcParser.cpp
 * Datum: 1. 10. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Zdrojovy soubor pro parsovani IRC prikazu.
 */

#include "IrcParser.h"

/*
 * Metoda, ktera zacne zpracovavani prikazu (radky).
 * @param line Radka (prikaz), ktery se ma zpracovat.
 */
void IrcParser::ProcessLine(const char * line)
{
   state = ERROR;
   value = command = prefix = "";
   this->line = string(line);
   lineIndex = 0;
   
   Message();   
}

/*
 * Metoda vracejici dalsi znak zpracovavaneho prikazu.
 * @return Dalsi znak, END v pripade konce prikazu.
 */
int IrcParser::GetNextChar()
{
   if(lineIndex >= line.length())
   {
      return END;
   }
   return line[lineIndex++];
}

/*
 * Metoda, ktera zpracuje zpravu na zaklade gramatiky v RFC. Nedochazi 
 * ke kontrole spravnosti formatu zpravy (predpoklada se validni 
 * IRC server).
 * @return Uspesnost zpracovani.
 */
bool IrcParser::Message()
{
   int ch = GetNextChar();
   if(ch == END)
      return false;
   if(ch == ':')
   {
      if(!Prefix()) return false;
      
      ch = GetActChar();
      if(ch != ' ') return false;
      do
      {
         ch = GetNextChar();
         if(ch == END)
            return false;
      } while(ch == ' ');
   }
   
   if(!Command()) return false;
   if(!Params()) return false;
   
   if(GetActChar() != '\n') return false;
   
   return true;
}

/*
 * Metoda, ktera zpracuje polozku zpravy <command> a prikaz ulozi 
 * do instancni promenne.
 * @return Uspesnost zpracovani.
 */
bool IrcParser::Command()
{
   int ch = GetActChar();
   command.clear();
   
   if(isalpha(ch))
   {
      while(isalpha(ch))
      {
         command.push_back(ch);
         ch = GetNextChar();
         if(ch == END)
            return false;
      }
      
      return true;
   }
   else if(isdigit(ch))
   {
      while(isdigit(ch))
      {
         command.push_back(ch);
         ch = GetNextChar();
         if(ch == END)
            return false;
      }

      return true;
   }
   else
      return false;
}

/*
 * Metoda, ktera zpracuje polozku zpravy <params> a parametry ulozi 
 * do instancni promenne.
 * @return Uspesnost zpracovani.
 */
bool IrcParser::Params()
{
   int ch = GetNextChar();
   value.clear();
   
   while(ch != '\n' && ch != '\r')
   {
      value.push_back(ch);
      ch = GetNextChar();
      if(ch == END)
         return false;
   }
      
   return true;
}

/*
 * Metoda, ktera zpracuje polozku zpravy <command> a prikaz ulozi 
 * do instancni promenne.
 * @return Uspesnost zpracovani.
 */
bool IrcParser::Prefix()
{
   int ch = GetActChar();
   prefix.clear();
   
   while(ch != ' ')
   {
      prefix.push_back(ch);
      ch = GetNextChar();
      if(ch == END)
         return false;
   }
   
   return true;
}

/*
 * Metoda nacitajici nick z prefixu zpravy.
 * @return Vyseparovany nick.
 */
string IrcParser::GetNickFromPrefix(string prefix)
{
   string host = "";
   for(unsigned int i = 1; i < prefix.length(); i++)
   {
      if(prefix[i] == '!' || prefix[i] == '@')
         return host;
      host.push_back(prefix[i]);
   }
   return host;
}

/*
 * Metoda, ktera nacte zpravu z parametru.
 * @return Vyseparovana zpravy z parametru.
 */
string IrcParser::GetMessageFromParams(string params)
{
   unsigned int i;
   for(i = 0; i < params.length(); i++)
   {
      if(params[i] == ' ')
      {
         i++;
         break;
      }
   }
   while(i < params.length() && params[i] == ' ') i++;
   
   if(i < params.length())
   {
      if(params[i] == ':')
         i++;
         
      return params.substr(i);
   }
   else
   {
      return "";
   }
}
