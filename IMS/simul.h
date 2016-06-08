
/*
 * Soubor: simul.h
 * Autor: Vojtech Havlena (xhavle03), Karel Brezina (xbrezi13)
 * 
 * Hlavickovy soubor nesouci konstanty pouzite pro experimenty a 
 * pro validacni experiment.
 */

#ifndef _SIMUL_H_
#define _SIMUL_H_

#define UNDEFINED 100000

/*  
 * Importy plynu do jednotlivych zemi (za hodinu) podle skutecnosti.
 * Pouzito pro ucely validace modelu.
 */
#define RUUK 2949
#define RUHU 933
#define RUSK 628
#define RUBE 2312
#define RUPO 1099
#define RUCZ 969
#define RUGE 4597
#define RUOUT (24054 - RUGE - RUCZ - RUPO - RUBE - RUSK - RUHU - RUUK)
#define KARU 935

#define HUUK 126
#define CZPO 66

#define NOCZ 4
#define NOGE (2268 + NOCZ)
#define NEGE 3246

#define GEPO 259
#define GEOUT 1890


/* Pomery letnich spotrep -- zimni = 2 - letni */
#define CZECH_SUM  0.639
#define GER_SUM  0.78
#define POLAND_SUM  0.805
#define SLOVAK_SUM  0.637
#define HUNGARY_SUM  0.557
#define UKRAINE_SUM  0.798
#define BELARUS_SUM  0.82
#define RUSSIA_SUM 0.812 

/* Indexy do pole parametru. */
enum PARINDEX
{
   ZEEBRUGGE = 0,
   ROTTERDAM,
   NSCORIDOR,
   USA,
   KOLPIPE,
   SNPIPE,
   ZEPIPE,
   ROPIPE,
   STRAT
};

#endif

