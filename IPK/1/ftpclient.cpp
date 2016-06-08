
/*
 * Soubor: ftpclient.c
 * Datum: 1. 3. 2014
 * Autor: Vojtech Havlena, xhavle03
 * 
 * Program slouzici pro pripojovani na ftp servery a 
 * ziskavani obsahu vzdalenych adresaru.
 */

#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>

#include "Url.h"

/* Chybove kody. */
enum ERRORS {
   EOK,
   ECONNECT,
   EHOST,
   ESOCKET,
   ESEND,
   ELIST,
   ERESP,
   EADDR,
   EPARAMS
};

//#define PRINT_RESPONSE

#define COMMAND_COUNT 6
#define CWD_INDEX 2
#define USER_INDEX 0
#define PASS_INDEX 1

using std::string;
using std::cout;
using std::cerr;
using std::endl;

/* Chybove zpravy programu. */
string ERROR_MSG[] = {
   "V poradku.", "Chyba pri navazovani spojeni.",
   "Zadana neplatna adresa.", "Chyba pri vytvareni soketu.",
   "Chyba pri odesilani dat.", 
   "Chyba pri ziskavani vypisu obsahu adresare.",
   "Chybna odpoved serveru.",
   "Chybne formatovana adresa.",
   "Spatne parametry programu."
};

/* Prototypy funkci. */
int convertToInt(const char * buffer, unsigned int * start);
int getFileList(in_addr addr, unsigned short port, string *output);
bool connectToFileListEpsv(in_addr host, string & text, bool &request);
bool connectToFileListPasv(string & text);
void PrintError(ERRORS code);

/* Funkce main. */
int main(int argc, char *argv[])
{
   string messages[COMMAND_COUNT] = {"USER anonymous\r\n", "PASS \r\n", 
         "CWD ", "PASV\r\n", "LIST\r\n", "QUIT\r\n"};
   string reply[COMMAND_COUNT + 1] = {"220", "331", "230", "250", 
         "227", "150", "226"}; /* 229 - EPSV */
   string text;
   hostent *host;       
   sockaddr_in serverSock;    
   int mySocket;            
   int size;       
   
   if(argc != 2)
   {
      PrintError(EPARAMS);
      return EPARAMS;
   }
   string hostName(argv[1]);
   bool isPath = false;
   
   Url uri(hostName);
   uri.Parse();
   if(((uri.user != "" || uri.pass != "") && !uri.ftp) || (uri.user == "" && uri.pass != "") || (uri.user != "" && uri.pass == ""))
   {
      PrintError(EADDR);
      return EADDR;
   }
   
   if(uri.path != "" && uri.path != "/")
      isPath = true;
      
   if(uri.user != "")
      messages[USER_INDEX] = "USER " + uri.user + "\r\n";
   if(uri.pass != "")
      messages[PASS_INDEX] = "PASS " + uri.pass + "\r\n";
   hostName = uri.host;

   if ((host = gethostbyname(hostName.c_str())) == NULL)
   {
      PrintError(EHOST);
      return EHOST;
   }
   if ((mySocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      PrintError(ESOCKET);
      return ESOCKET;
   }

   serverSock.sin_family = AF_INET;
   serverSock.sin_port = htons(uri.port);
   memcpy(&(serverSock.sin_addr), host->h_addr, host->h_length);
   if (connect(mySocket, (sockaddr *)&serverSock, sizeof(serverSock)) == -1)
   {
      PrintError(ECONNECT);
      close(mySocket);
      return ECONNECT;
   }
   
   unsigned int counter = 0;
   string res = "";
   string txt = "";
   char input;
   bool request = false;

   int j = 0;
   while ((size = read(mySocket, &input, 1)) > 0) 
   {
      text.append(&input, 1);
      if(input != '\n')
         continue;
         
      counter = 0;
      while(counter < text.length() && isdigit(text[counter])) counter++;
      if(counter != 3)
      {
         text = "";
         continue;
      }
      if(text[counter] == ' ')
      {
         if(text.substr(0,3) != reply[j])
         {
            PrintError(ERESP);
            close(mySocket);
            return ERESP;
         }
         
         if(j == COMMAND_COUNT) break;
         if(j == CWD_INDEX)
         {
            if(isPath) messages[j].append(uri.path + "\r\n");
            else j++;
         }
         
         if ((size = send(mySocket, messages[j].c_str(), messages[j].length(), 0)) == -1)
         {
            PrintError(ESEND);
            close(mySocket);
            return ESEND;
         }
         j++;
      }
      
      #ifdef PRINT_RESPONSE
      cout << text;
      #endif
      
      size_t pos;
      if((pos = text.find("227")) != string::npos && !request && pos == 0)
      {
         if(!connectToFileListPasv(text))
         {
            PrintError(ELIST);
            close(mySocket);
            return ELIST;
            /* Pokusime se o rozsirene pasivni pripojeni. */
            /*messages[j] = "EPSV \r\n";
            reply[j + 1] = "229";
            j--;*/
         }
         request = true;
      }
      text = "";
   }
    
   close(mySocket);
   return 0;
}

/*
 * Funkce, ktera prevede retezec na cislo.
 * @param buffer Retezec, ktery se ma prevest.
 * @param start Zacatek retezce.
 * @return Prevedene cislo.
 */
int convertToInt(const char * buffer, unsigned int * start)
{
   int res = 0;
   while(isdigit(buffer[*start]))
   {
      res *= 10;
      res += (buffer[*start] - '0');
      (*start)++;
   }

   return res;
}

/*
 * Funkce, ktera se pripoji k serveru za ucelem ziskani vypisu 
 * obsahu adresare.
 * @param host Hostitelsky FTP server.
 * @param text Odpoved na pasivni rezim.
 * @param request Byl pozadavek splnen?
 */
bool connectToFileListEpsv(in_addr addr, string & text, bool &request)
{
   size_t pos = text.rfind("(|||");
   string number = "";
   string txt = "";
   
   if(pos != string::npos)
   {
      pos += 4;
      
      unsigned int i = (unsigned int)pos;
      int a = convertToInt(text.c_str(), &i);
      int error;
      
      if((error = getFileList(addr, a, &txt)) != EOK)
      {
         return false;
      }
      
      request = true;
      cout << txt;
   }
   
   return true;
}

/*
 * Funkce, ktera se pripoji k serveru za ucelem ziskani vypisu 
 * obsahu adresare.
 * @param text Odpoved serveru na PASV.
 * @return Uspech operace.
 */
bool connectToFileListPasv(string & text)
{
   size_t pos = text.find_last_of("(");
   if(pos == string::npos)
      return false;
   int parse[2];
   string ip = "";
   
   pos++;
   for(int paramCounter = 0; paramCounter < 6; paramCounter++)
   {
      if(paramCounter < 4)
      {
         while(pos < text.size())
         {
            if(!isdigit(text[pos]))
               break;
            ip.push_back(text[pos++]);
         }
         ip.push_back('.'); pos++;
      }
      else
      {
         parse[paramCounter - 4] = convertToInt(text.c_str(), (unsigned int *)&pos);
         pos++;
      }
   }
   if(ip.size() > 0)
      ip = ip.substr(0, ip.size() - 1);
   
   string txt = "";
   in_addr addr;
   addr.s_addr = inet_addr(ip.c_str());
   if(getFileList(addr, parse[0] * 256 + parse[1], &txt) != EOK)
   {
      return false;
   }
   cout << txt;
   
   return true;
}

/*
 * Funkce, ktera nacte obsah adresare vzdaleneho serveru.
 * @param host Hostitelsky FTP server.
 * @param port Port na kterem se ma uzivatel pripojit.
 * @param output Vystup.
 * @return Kod uspesnosti.
 */
int getFileList(in_addr addr, unsigned short port, string *output)
{
   sockaddr_in listenSock;
   int sock;
   
   if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
   {
      return ESOCKET;
   }
   
   bzero((char *) &listenSock, sizeof(listenSock));
   listenSock.sin_family = PF_INET;
   listenSock.sin_port = htons(port);

   /*struct timeval timeout;      
   timeout.tv_sec = 4;
   timeout.tv_usec = 0;
        
   if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
         sizeof(timeout)) < 0)
      return EHOST;*/
   
   //memcpy(&(listenSock.sin_addr), host->h_addr, host->h_length);
   listenSock.sin_addr = addr;
   if (connect(sock, (sockaddr *)&listenSock, sizeof(listenSock)) == -1)
   {
      close(sock);
      return ECONNECT;
   }

   int size = 0;
   char input;
   *output = "";
   while((size = recv(sock, &input, 1, 0)) > 0) 
   { 
      output->append(&input, 1);
   }
   close(sock);
   
   return EOK;
}

/*
 * Funkce, ktera vytiskne chybovou hlasku.
 * @param code Chybovy kod.
 */
void PrintError(ERRORS code)
{
   cerr << ERROR_MSG[code] << endl;
}
