
/*
 * Soubor: lngTerminal.cpp
 * Autor: Vojtech Havlena (xhavle03), Karel Brezina (xbrezi13)
 * 
 * Zdrojovy soubor k implmentaci LNG terminalu.
 */

#include "lngTerminal.h"

/*
 * Metoda, ktera provede jednu hodinu cinnosti LNG terminalu.
 * @param time Aktualni cas simulace.
 */
void LngTerminal::ProcessHour(long time)
{
   int sendGas = 0;
   for (auto &val : sendStates) 
   {      
      sendGas = val.first;
      
      if(sendGas == -1)
         sendGas = capacity;
      if(sendGas > capacity)
         sendGas = capacity;
      if(sendGas > actStorage)
         sendGas = actStorage;
      if(sendGas > val.second->GetCapacity())
         sendGas = val.second->GetCapacity();
         
      val.second->AddSendGas(sendGas);
      val.second->SendGas(time);
      
      actStorage -= sendGas;
   }
}

/*
 * Metoda, ktera ma byt volana po kazde hodine, dochazi k aktualizaci 
 * informaci o LNG terminalu.
 */
void LngTerminal::CheckStatus()
{
   actStorage = production;
}
