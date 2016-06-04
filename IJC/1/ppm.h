/*
 * steg-decode.c
 * Reseni IJC-DU, priklad b), 5. 3. 2012
 * Autor: Vojtech Havlena (xhavle03), FIT
 * Prelozeno: GCC 4.7.2
 */

#ifndef PPM_H_
#define PPM_H_

#define COLOR_DEEP 255
#define ERROR -1
#define OK 0

/* Struktura pro ulozeni struktury nacteneho PPM obrazku */
struct ppm 
{ 
   unsigned xsize; /* Rozmer x */
   unsigned ysize;  /* Rozmer y */
   char data[]; // RGB bajty, celkem 3*xsize*ysize 
};

struct ppm * ppm_read(const char * filename);
void ReleaseResources(struct ppm * img, FILE *fr);
int ppm_write(struct ppm *p, const char * filename);

#endif
