
/*
 * Soubor: ircbot.cpp
 * Datum: 1. 10. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Zdrojovy soubor obsahujici samotnou implementaci ircbota. Podrobnejsi 
 * info o implementaci v dilcich souborech.
 */

#include "IrcParser.h"
#include "ircbot.h"

/* Debugovaci vystup. */
//#define DEBUG_OUT

using namespace std;

SyslogHandler syslogHandler;
int sock;

/*
 * Vstupni funkce programu.
 * @param argc Pocet argumentu.
 * @param argv Jednotlive argumenty.
 * @return Uspesnost vykonani programu.
 */
int main(int argc, char *argv[])
{
   PARAMS params;
   signal(SIGINT, SignalHandler);
   
   if(!ParseParams(argc, argv, &params))
   {
      PrintError(EPARAMS, false);
      return EPARAMS;
   }

   hostent *host; 
   sockaddr_in serverSock;
   
   /* Spojeni s logovacim serverem. */
   if(!syslogHandler.Connect(params.syslogServer))
   {
      PrintError(ESYSLOG, false);
      return ESYSLOG;
   }
   
   if ((host = gethostbyname(params.host)) == NULL)
   {
      PrintError(EHOST, true);
      syslogHandler.Disconnect();
      return EHOST;
   }
   
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      PrintError(ESOCKET, true);
      syslogHandler.Disconnect();
      return ESOCKET;
   }

   
   /*int one = 1;
   int keepcnt = 2;
   cout << setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one));
   int idletime = 120;
   cout << setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(int));
   cout << setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idletime, sizeof(idletime));*/
   
   serverSock.sin_family = AF_INET;
   serverSock.sin_port = htons(params.port);
   memcpy(&(serverSock.sin_addr), host->h_addr, host->h_length);
   /* Spojeni s IRC serverem. */
   if (connect(sock, (sockaddr *)&serverSock, sizeof(serverSock)) == -1)
   {
      PrintError(ECONNECT, true);
      close(sock);
      syslogHandler.Disconnect();
      return ECONNECT;
   }

   /* Zaslani prikazu na IRC server. */
   int size;
   commands[2].append(params.channel); commands[2].append("\r\n");
   for(int i = 0; i < COMMANDS; i++)
   {
      if ((size = send(sock, commands[i].c_str(), commands[i].length(), 0)) == -1)
      {
         PrintError(ESEND, true);
         close(sock);
         syslogHandler.Disconnect();
         return ESEND;
      }
   }
   
   /* Cteni prichozich zprav z IRC serveru. */
   bool suc = ReadIrcMessages(sock, &params, &syslogHandler);
   
   CloseConnection();
   
   return suc ? 0 : 1;
}

/*
 * Funkce, ktera cte jednotlive zpravy prichazejici z IRC serveru. 
 * Ve zpravach hleda PRIVMSG a NOTICE a pritomnost klicovych slov.
 * @param sock Socket pripojeni s IRC.
 * @param params Spravce pripojeni s logovacimu serveru.
 * @return Uspesnost vykonani.
 */
bool ReadIrcMessages(int sock, PARAMS *params, SyslogHandler *handler)
{
   IrcParser parser;
   char input;
   string line = "";
   string pong, nick, mess, command;
   unsigned int index;
   
   while (read(sock, &input, 1) > 0) 
   {
      line.append(&input, 1);
      if(input != '\n')
         continue;
      
      #ifdef DEBUG_OUT
      cout << line << endl;
      #endif

      parser.ProcessLine(line.c_str());
      command = parser.GetCommand();
      if(command == "PING")
      {
         mess = parser.GetValue();
         index = 0;
         while(index < mess.length())
         {
            if(isspace(mess[index]))
               break;
            index++;
         }
         mess = mess.substr(0, index);
         
         pong = "PONG " + mess + "\r\n";
         if (send(sock, pong.c_str(), pong.length(), 0) == -1)
         {
            PrintError(ESEND, true);
            close(sock);
            return false;
         }
      }
      /* Prijata zprava */
      else if(command == "PRIVMSG" || command == "NOTICE")
      {
         nick = IrcParser::GetNickFromPrefix(parser.GetPrefix());
         mess = IrcParser::GetMessageFromParams(parser.GetValue());
         
         #ifdef DEBUG_OUT
         cout << nick << " poslal zpravu: " << mess << endl;
         #endif
         
         if(ContainsMessageWord(mess, params->words))
         {
            if(!handler->SendLog(nick, mess))
            {
               PrintError(ELOGMESS, false);
               return false;
            }
         }
      }
      else if(IsError(command))
      {
         mess = parser.GetValue();
         mess = ParseError(mess);
         if(!handler->SendLog("", mess))
         {
            PrintError(ELOGMESS, false);
         }
         return false;
      }
      
      line.clear();
   }
   
   return true;
}

/*
 * Funkce, ktera zpracuje parametry programu.
 * @param argc Pocet argumentu.
 * @param argv Jednotlive argumenty.
 * @param params Struktura pro vyplneni.
 * @return Validita parametru.
 */
bool ParseParams(int argc, char *argv[], PARAMS *params)
{
   if(argc < 4 || argc > 5)
      return false;
      
   char *host = argv[1];
   int i, hostLength = strlen(host);
   bool port = false;
   
   for(i = hostLength - 1; i >= 0; i--)
   {
      if(!isdigit(host[i]))
         break;
      if(host[i] == ':')
      {
         if(i != hostLength - 1)
            port = true;
         break;
      }
   }
   
   if(port)
   { 
      host[i] = '\0';
      params->port = atoi(host + i + 1);
   }
   else
   {
      params->port = 6667;
   }
   params->host = host;
   params->channel = argv[2];
   params->syslogServer = argv[3];
   
   if(argc == 4)  
      return true;
   
   char *words = argv[4];
   int wordsLength = strlen(words);
   string each = "";
   for(int j = 0; j < wordsLength; j++)
   {
      if(words[j] == ';')
      {
         params->words.push_back(each);
         each.clear();
         continue;
      }
      each.push_back(words[j]);
   }
   if(!each.empty())
      params->words.push_back(each);
   
   return true;
}

/*
 * Funkce, ktara kontroluje, zda se ve zprave nachazi nejake 
 * hledane slovo.
 * @param mess Zprava.
 * @words Seznam hledanych slov.
 * @return Obsahuje zprava nejake slovo.
 */
bool ContainsMessageWord(string &mess, std::vector<string> &words)
{
   if(words.size() == 0)
      return true;
   for(unsigned int i = 0; i < words.size(); i++)
   {
      if(mess.find(words[i]) != string::npos)
         return true;
   }
   return false;
}

/*
 * Funkce, ktera z parametru chybove zpravy vyparsuje samotnou chybovou 
 * hlasku bez parametru.
 * @param error Chybova zprava
 * @return Chybova hlaska
 */
string ParseError(string &error)
{
   size_t pos = error.find(':');
   if(pos == string::npos)
   {
      pos = error.find_last_of(' ');
      if(pos == string::npos)
         pos = -1;
   }
   
   return error.substr(pos + 1);
}

/*
 * Funkce, ktara kontroluje, zda cislo poslane jako 
 * prikaz je cislo chyby.
 * @param command Prikaz ze zpravy.
 * @return Jedna se o chybu?.
 */
bool IsError(string &command)
{
   for(unsigned int i = 0; i < command.length(); i++)
   {
      if(!isdigit(command[i]))
         return false;
   }
   
   int num = stoi(command);
   /*for(int i = 0; i < REPLIES; i++)
   {
      if(ERR_REPLY[i] == num)
         return true;
   }*/
   
   if(num >= 400)
      return true;
   
   return false;
}

/*
 * Funkce, ktera vypise chybovou zpravu na zaklade chyboveho kodu.
 * @param Chybovy kod.
 */
void PrintError(ERRORS code, bool log)
{
   cerr << ERROR_MSG[code] << endl;
   
   if(log)
   {
      if(!syslogHandler.SendLog("", ERROR_MSG[code]))
      {
         cerr << ERROR_MSG[ELOGMESS] << endl;
      }
   }
}

/*
 * Funkce, ktera se stara o obsluhu signalu. Pri signalu SIGINT se 
 * provede korektni uzavreni spojeni se servery.
 */
void SignalHandler(int signo)
{
   if (signo == SIGINT || signo == SIGTERM)
   {
      CloseConnection();
   }

   exit(2);
}

/*
 * Funkce uzavirajici spojeni se servery. V pripade IRC serveru
 * se jeste odesila zprava QUIT.
 */
void CloseConnection()
{
   send(sock, commands[3].c_str(), commands[3].length(), 0);
   syslogHandler.Disconnect();
   close(sock);
}
