
/*
 * Soubor: gasPipeline.cpp
 * Autor: Vojtech Havlena (xhavle03), Karel Brezina (xbrezi13)
 * 
 * Zdrojovy soubor k implementaci tridy reprezentujici plynovody.
 */

#include "gasPipeline.h"

/*
 * Metoda, ktera posle urcite mnozstvi plynu plynovodem a jeho douruceni 
 * ulozi do kalendare podle delky plynovodu.
 * @param time Aktualni cas.
 */
void GasPipeline::SendGas(long time)
{
   if(to == NULL)
   {
      from->SetStored(from->GetStored() - send);
      send = 0;
      return;
   }
   
   if(send > capacity)
   {
      send = capacity;
   }
   
   n++;
   average = (average * (n-1) + send) / (double)n;
   
   if(from != NULL)
   {
      from->SetStored(from->GetStored() - send);
   }

   /* Ulozeni doruceni do kalendare. */
   CalItem item;
   item.gas = send;
   item.to = to;
   item.time = time + length / GASSPEED;
   calendar->push(item);
     
   send = 0;
}
