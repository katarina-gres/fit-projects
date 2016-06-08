
/*
 * Soubor: state.cpp
 * Autor: Vojtech Havlena (xhavle03), Karel Brezina (xbrezi13)
 * 
 * Zdrojovy soubor k implementaci tridy reprezentujici stat.
 */

#include "state.h"

/*
 * Metoda, ktera provede hodinu chodu statu (spotrebuje se urcite 
 * mnozstvi plynu a urcite mnozstvi se vyprodukuje).
 */
void State::ProcessHour()
{
   long cons = consumption * ((100 - CONSUMPTION ) + rand() % (CONSUMPTION * 2)) / 100.0;
   if(summerConsumption)
   {
      cons = consumption * summerRatio;
   }
   else
   {
      cons = consumption * (2 - summerRatio);
   }
   
   stored += importOther + production * ((100 - PRODUCTION ) + rand() % (PRODUCTION * 2)) / 100.0;
   stored -= cons;
}

/*
 * Metoda, ktera exportuje plyn ze statu.
 * @param strategy Pouzita strategie.
 * @param time Aktualni cas.
 */
void State::SendGas(STRATEGY strategy, long time)
{
   long im = 0;
   if(strategy == STRATEGY::GREEDY)
   {
      /* Mnozstvi odesilaneho plynu. */
      im = importLast - consumption + production + importOther;
      
      if(im < 0)
         return;
      
      if(sendStates == NULL)
         return;
      for (auto &val : *sendStates) 
      {
         /* Mnozstvi plynu je rozdeleno podle pomeru kapacit. */
         val.second->AddSendGas(im * (val.second->GetCapacity() / (double)sumOutput));
         val.second->SendGas(time);
      }
   }
   else
   {
      if(sendStates == NULL)
         return;
      for (auto &val : *sendStates) 
      {
         val.second->AddSendGas(val.first);
         val.second->SendGas(time);
      }
   }
}

/*
 * Metoda, ktera aktualizuje mnozstvi plynu (po kazde hodine).
 */
void State::CheckStatus()
{
   stored += importGas;
   importLast = importGas;
   importGas = 0;
   
   if(stored < 0)
   {
      stored = 0;
   }
   if(stored > capacity)
      stored = capacity;
   
   n++;
   average = (average * (n-1) + stored) / (double)n;
}

/*
 * Nastaveni statu, kam se ma posilat plyn.
 * @param send Staty, kam se ma posilat plyn.
 */
void State::SetSendStates(map<long, GasPipeline*> * send)
{
   sendStates = send;
   sumOutput = 0;
   for (auto &val : *sendStates) 
   {
      sumOutput += val.second->GetCapacity();
   }
}
