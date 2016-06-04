/*
 * steg-decode.c
 * Reseni IJC-DU, priklad b), 5. 3. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#include <stdlib.h>
#include <stdio.h>

#include "ppm.h"
#include "error.h"

/*  
 * Funkce, ktera nacte obrazek ve formatu PPM do struktury ppm, ktera 
 * slouzi pro manipulaci s jednotlivymi barevnymi body  obrazku.
 * param filename Cesta k nacitanemu obrazku
 * return ppm, v pripade chyby NULL
 */
struct ppm * ppm_read(const char * filename)
{
   char ppmType[5] = {0};
   int colDeep;
   unsigned int xsize = 0;
   unsigned int ysize = 0;
   struct ppm *ret = NULL;
   
   FILE *fr = fopen(filename, "rb");
   if(fr == NULL)
   {
      ReleaseResources(ret, fr);
      Error("Chyba pri otevirani souboru."); //return NULL;
      return NULL;
   }
   
   if(fscanf(fr, "%3s", ppmType) != 1)
   {
      ReleaseResources(ret, fr);
      Error("Nepodarilo se precist variantu PPM souboru.");
      return NULL;
   }
   if(ppmType[0] != 'P' || ppmType[1] != '6' || 
      ppmType[2] != '\0')
   {
      ReleaseResources(ret, fr);
      Error("Neznama varianta PPM souboru."); 
      return NULL;
   }
      
   if(fscanf(fr, "%u", &xsize) != 1)
   {
      ReleaseResources(ret, fr);
      Error("Nepodarilo se precist x-ovy rozmer obrazku.");
      return NULL;
   }
   if(fscanf(fr, "%u", &ysize) != 1)
   {
      ReleaseResources(ret, fr);
      Error("Nepodarilo se precist y-ovy rozmer obrazku.");
      return NULL;
   }
   
   ret = malloc(sizeof(struct ppm) + 3 * xsize * ysize);
   if(ret == NULL)
   {
      ReleaseResources(ret, NULL);
      Error("Chyba pri alokovani pameti."); //return NULL;
      return NULL;
   }
   ret->xsize = xsize; ret->ysize = ysize;
   
   if(fscanf(fr, "%d%*c", &colDeep) != 1)
   {
      ReleaseResources(ret, fr);
      Error("Nepodarilo se precist bitovou hloubku.");
      return NULL;
   }
   if(colDeep != COLOR_DEEP)
   {
      ReleaseResources(ret, fr);
      Error("Nepodporovana barevna hloubka.");
      return NULL;
   }
      
   int read;
   unsigned long index = 0;
   while((read = getc(fr)) != EOF)
   {
      if(index >= ret->xsize * ret->ysize * 3)
      {
         ReleaseResources(ret, fr);
         Error("Pocet bytu neodpovida rozliseni obrazku."); 
         return NULL;
      }
      ret->data[index++] = (char)read;
   }
   
   if(fclose(fr) == EOF)
   {
      ReleaseResources(ret, fr);
      Error("Problem pri uzavirani souboru."); 
      return NULL;
   }
   
   return ret;
}

/*
 * Funkce, ktera zapise obsah struktury ppm do souboru.
 * param p Nacteny obrazek ve formatu PPM
 * param filename Jmeno souboru pro ulozeni
 * return Uspesnost provedeni
 */
int ppm_write(struct ppm *p, const char * filename)
{
   FILE *fw = fopen(filename, "wb");
   if(fw == NULL)
   {
      Error("Chyba pri vytvareni souboru.");
      return ERROR;
   }
   
   fprintf(fw, "P6\n%u %u\n255\n", p->xsize, p->ysize);
   unsigned long biteCount = p->xsize * p->ysize * 3;
   for(unsigned long i = 0; i < biteCount; i++)
   {
      fputc(p->data[i], fw);
   }
   
   if(fclose(fw) == EOF)
   {
      Error("Chyba pri uzavirani souboru.");
      return ERROR;
   }
   return OK;
}

/*
 * Funkce, ktera uvolni zdroje.
 */
void ReleaseResources(struct ppm * img, FILE *fr)
{
   if(img != NULL)
   {
      free((void *)img);
   }
   if(fr != NULL)
   {
      fclose(fr);
   }
}
