
/*
 * Soubor: simul.cpp
 * Autor: Vojtech Havlena (xhavle03), Karel Brezina (xbrezi13)
 * 
 * Implementace projektu do predmetu IMS -- Plynova krize v Evrope.
 * Casovou jednotkou je 1 hodina (doba simulace 2 roky).
 * 
 * Parametrem programu je cislo experimentu, ktery se ma provest.
 *    -- 0-3 experimenty popsane v dokumentaci
 *    -- 4 experiment slozici pro validaci (smluvni dodavky plynu)
 */

#include <vector>
#include <time.h>  
#include <cstdlib> 
#include <iomanip>

#include "state.h"
#include "lngTerminal.h"
#include "simul.h"

using std::vector;

/*
 * Parametry pro jednotlive experimenty.
 */
int params[][9] = {
      {1027, 1369, 0, 0, 1916, 2958, 1655, 1655, STRATEGY::GREEDY},
      {1027*2, 1369*2, 0, 0, 6000, 3500, 1655+1507, 1655+1507, STRATEGY::GREEDY},
      {1027*2, 1369*2, 1, 0, 6000, 3500, 1655+1507, 1655+1507, STRATEGY::GREEDY},
      {1027*2, 1369*2, 1, 1, 6000, 3500, 1655 + 1507, 1655 + 1507, STRATEGY::GREEDY},
      {1027, 1369, 0, 0, 1916, 2958, 1655, 1655, STRATEGY::CONTRACT}};

/*
 * Hlavni funkce.
 * @param argc Pocet parametru.
 * @param argv Jednotlive parametry.
 */
int main(int argc, char *argv[])
{
   if(argc != 2)
   {
      cerr << "Spatne parametry programu (--help pro napovedu)" << endl;
      return 1;
   }
   if(strcmp(argv[1], "--help") == 0)
   {
      cout << "Parametry programu: e -- cislo experimentu" << endl;
      return 0;
   }
   
   char * pEnd;
   long exp = strtol(argv[1], &pEnd, 10);
   if(*pEnd != '\0' || exp < 0 || exp > 4)
   {
      cerr << "Spatne parametry programu (--help pro napovedu)" << endl;
      return 1;
   }
   
   /* Jednotlive staty. */
   State russia("Rusko", 53289, 76584, 70400000, RUSSIA_SUM);
   State ukraine("Ukrajina", 5875, 2288, 31950000, UKRAINE_SUM);
   State slovak("Slovensko", 664, 0, 3020000, SLOVAK_SUM);
   State belarus("Belorusko", 2338, 27, 2832000, BELARUS_SUM);
   State hungary("Madarsko", 1041, 222, 6330000, HUNGARY_SUM);
   State poland("Polsko", 2081, 709, 2524000, POLAND_SUM);
   State czech("Cesko", 968, 29, 3436000, CZECH_SUM);
   State germany("Nemecko", 10096, 1345, 22027000, GER_SUM);
   
   /* Nastaveni importu od ostatnich zemi -- pouze pro ucely validace. */
   if(exp == 4)
   {
      germany.SetImportOther(724);
      russia.SetImportOther(935);
   }

   /* Uvazovane LNG terminaly. */
   LngTerminal kollsness("Kollsness", 5958, 5958);
   LngTerminal snurrevarden("Snurrevarden", 3208, 3208);
   LngTerminal zeebrugge("Zeebrugge", params[exp][ZEEBRUGGE], params[exp][ZEEBRUGGE]); /*1027*/
   LngTerminal rotterdam("Rotterdam", params[exp][ROTTERDAM], params[exp][ZEEBRUGGE]); /*1369*/
   
   LngTerminal swinoujscie("Swinoujscie", 854, 854);
   LngTerminal adriatic("Adriatic", 1708, 1708);
   LngTerminal usa("USA", 6500, 6500);
   
   /* Ulozeni vsech statu a terminalu do kolekce -- jake se 
    * pouziji rozhoduje az konkretni experiment. */
   vector<State *> states = { &ukraine, &slovak, &belarus, &hungary, 
      &poland, &czech, &germany };
   if(exp == 4)
   {
      states.push_back(&russia);
   }
   vector<LngTerminal *> terminals = { &kollsness, &snurrevarden, 
      &zeebrugge, &rotterdam };
   if(params[exp][NSCORIDOR])
   {
      terminals.push_back(&swinoujscie);
      terminals.push_back(&adriatic);
   }
   if(params[exp][USA])
   {
      terminals.push_back(&usa);
   }
   
   /* Kalendar pro ukladani dorucovaneho plynu. */
   CALENDAR calendar;
   
   /* Jednotlive plynovody. */
   GasPipeline RuUk(&russia, &ukraine, 8542, 775, &calendar);
   GasPipeline RuBe(&russia, &belarus, 3843, 695, &calendar);
   GasPipeline RuGe(&russia, &germany, 6539, 1222, &calendar);
   GasPipeline RuOut(&russia, NULL, UNDEFINED, 0, &calendar);
   GasPipeline UkSl(&ukraine, &slovak, 8542, 1010, &calendar);
   
   GasPipeline BePo(&belarus, &poland, 3843, 480, &calendar);
   GasPipeline SkCz(&slovak, &czech, 1978, 300, &calendar);
   
   GasPipeline SkUk(&slovak,&ukraine, 8542, 1010, &calendar);
   GasPipeline UkHu(&ukraine, &hungary, 2232, 905, &calendar);
   
   GasPipeline HuOut(&hungary, NULL, 1000, 0, &calendar);
   GasPipeline HuUk(&hungary, &ukraine, 2232, 905, &calendar);
   GasPipeline HuSk(&hungary, &slovak, 1640, 165, &calendar);
   
   GasPipeline PoBe(&poland, &belarus, 3843, 480, &calendar);
   GasPipeline PoUk(&poland, &ukraine, 506, 691, &calendar);
   GasPipeline CzSk(&czech, &slovak, 1978, 300, &calendar);
   GasPipeline CzPo(&czech, &poland, 104, 525, &calendar);
   
   GasPipeline GeCz(&germany, &czech, 1197, 285, &calendar);
   GasPipeline GePo(&germany, &poland, 1942, 517, &calendar);
   GasPipeline GeOut(&germany, NULL, 1000, 0, &calendar);
   
   /* Plynovody od LNG terminalu. */
   GasPipeline LNGKoGe(NULL, &germany, params[exp][KOLPIPE], 670, &calendar); 
   GasPipeline LNGSnGe(NULL, &germany, params[exp][SNPIPE], 670, &calendar); 
   GasPipeline LNGZeGe(NULL, &germany, params[exp][ZEPIPE], 590, &calendar); 
   GasPipeline LNGRoGe(NULL, &germany, params[exp][ROPIPE], 609, &calendar); 
   GasPipeline LNGSwPo(NULL, &poland, 1640, 100, &calendar);
   GasPipeline LNGAdHu(NULL, &hungary, 1640, 359, &calendar);
   GasPipeline LNGUsPo(NULL, &poland, 3000, 100, &calendar);
   map<long, GasPipeline*> koMap = {{-1, &LNGKoGe}};
   map<long, GasPipeline*> snMap = {{-1, &LNGSnGe}};
   map<long, GasPipeline*> zeMap = {{-1, &LNGZeGe}};
   map<long, GasPipeline*> roMap = {{-1, &LNGRoGe}};
   if(exp == 4) /* Opet pro ucely validace modelu (odesilani realneho/smluvniho mnozstvi plynu) */
   {
      koMap = {{NOGE / 2, &LNGKoGe}};
      snMap = {{NOGE / 2, &LNGSnGe}};
      zeMap = {{NEGE / 2, &LNGZeGe}};
      roMap = {{NEGE / 2, &LNGRoGe}};
   }
   
   kollsness.sendStates = koMap;
   snurrevarden.sendStates = snMap;
   zeebrugge.sendStates = zeMap;
   rotterdam.sendStates = roMap;
   
   map<long, GasPipeline*> swMap = {{-1, &LNGSwPo}};
   swinoujscie.sendStates = swMap;
   map<long, GasPipeline*> adMap = {{-1, &LNGAdHu}};
   adriatic.sendStates = adMap;
   map<long, GasPipeline*> usMap = {{-1, &LNGUsPo}};
   usa.sendStates = usMap;

   /* Pripojeni plynovodu k jednotlivym statu. */
   map<long, GasPipeline*> ukMap = {{-1, &UkHu}};
   map<long, GasPipeline*> huMap = {{0, &HuOut}, {1, &HuUk}, {2, &HuSk}};
   map<long, GasPipeline*> poMap = {{-1, &PoBe}, {0, &PoUk}};
   map<long, GasPipeline*> skMap = {{-1, &SkUk}};
   map<long, GasPipeline*> czMap = {{0, &CzPo}, {1, &CzSk}};
   map<long, GasPipeline*> geMap = {{0, &GeCz}, {1, &GePo}, {2, &GeOut}};
   map<long, GasPipeline*> rusMap;
   map<long, GasPipeline*> beMap;
   if(exp == 4)
   {
      rusMap = {{RUUK + RUHU + RUSK + RUCZ, &RuUk}, {RUBE + RUPO, &RuBe}, {RUGE, &RuGe}, {RUOUT, &RuOut}};
      ukMap = {{RUSK + RUCZ, &UkSl}, {RUHU, &UkHu}};
      huMap = {{38, &HuOut}, {HUUK, &HuUk}};
      beMap = {{RUPO, &BePo}};
      skMap = {{RUCZ, &SkCz}};
      czMap = {{CZPO, &CzPo}};
      geMap = {{NOCZ, &GeCz}, {GEPO, &GePo}, {GEOUT, &GeOut}};
      beMap = {{RUPO, &BePo}};
   }
   russia.SetSendStates(&rusMap);
   ukraine.SetSendStates(&ukMap); 
   hungary.SetSendStates(&huMap);
   poland.SetSendStates(&poMap);
   slovak.SetSendStates(&skMap);
   czech.SetSendStates(&czMap);
   germany.SetSendStates(&geMap);
   belarus.SetSendStates(&beMap);
   
   map<string, vector<int>> exportData = { 
      {"Ukrajina", vector<int>()}, {"Nemecko", vector<int>()}, {"Slovensko", vector<int>()}, 
      {"Belorusko", vector<int>()}, {"Madarsko", vector<int>()}, 
      {"Polsko", vector<int>()}, {"Cesko", vector<int>()}};
   if(exp == 4)
   {
      exportData.insert(exportData.begin(), pair<string, vector<int>>("Rusko",vector<int>()));
   }
   
   srand(time(NULL));

   int start = 0;
   /* Doba simulace */
   int end = 24*30*12*2; //2 roky
   int time = 0;
   
   CalItem top;
   
   for(int i = 1; i <= end; i++)
   {
      /* Cinnost terminalu. */
      for(auto &val : terminals)
      {
         val->ProcessHour(i);
      }
      /* Cinnost statu. */
      for(auto &val : states)
      {
         val->ProcessHour();
      }
      /* Odeslani plynu. */
      for(auto &val : states)
      {
         val->SendGas((STRATEGY)params[exp][STRAT], i);
      }
      /* Aktualizace hodnot, ulozeni stavu zasobniku. */
      for(auto &val : states)
      {
         val->CheckStatus();
         if(i % (24*30) == 0)
         {
            exportData[val->Name()].push_back((long)val->GetStored());  
            //val->ClearAverage();          
         }
         if(i % (24*30*6) == 0 && i > 0)
         {
            val->SetSummerConsumption(!val->GetSummerConsumption());
            if(usa.GetCapacity() == 6500)
               usa.SetCapacity(0);
            else
               usa.SetCapacity(6500);
         }
      }
      if(i % (24*30) == 0)
      {
         time++;
      }
      
      /* Doruceni plynu podle kalendare. */
      while(calendar.size() > 0 && calendar.top().time == i)
      {
         top = calendar.top();
         
         calendar.pop();
         
         top.to->SetImportVal(top.to->GetImportVal() + top.gas);
      }
      /* Aktualizace informaci o LNG terminalech. */
      for(auto &val : terminals)
      {
         val->CheckStatus();
      }
   }
   
   /* Vystup je uspusoben jednoduchemu zpracovani pomoci gnuplotu. */
   cout << "Mesic\t";
   for(auto &a : exportData)
   {
      cout << setw(10) << a.first << "\t";
   }
   cout << endl;
   for(int i = 0; i < time; i++)
   {
      cout << i + 1 << "\t";
      for(auto &a : exportData)
      {
         cout << setw(10) << a.second[i] << "\t";
      }
      cout << endl;
   }
   
   /*for(auto &a : exportData)
   {
      cout << a.first << ": ";
      for(auto &b : (a.second))
      {
         cout << b << ", ";
      }
      cout << endl;
   }*/
   
   return 0;
}
