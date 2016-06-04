
#
# Makefile pro 1. domaci ukol do predmetu IJC
# Autor: Vojtech Havlena, xhavle03
#

CC=gcc
CFLAGS=-O2 -std=c99 -Wall -pedantic
PROJ=du1

ERASTOTHENES=eratosthenes.c
PRVOCISLA=prvocisla.c
ERROR=error.c
STEG-DECODE=steg-decode.c
PPM=ppm.c

$(PROJ): prvocisla prvocisla-inline steg-decode

error.o: $(ERROR) error.h
	$(CC) $(CFLAGS) -c $(ERROR) -o $@ -lm

eratosthenes.o: $(ERASTOTHENES) error.h bit-array.h eratosthenes.h
	$(CC) $(CFLAGS) -c $(ERASTOTHENES) -o $@ -lm

prvocisla.o: $(PRVOCISLA) eratosthenes.h bit-array.h error.h
	$(CC) $(CFLAGS) -c $(PRVOCISLA) -o $@ -lm
	
ppm.o: $(PPM) bit-array.h error.h eratosthenes.h ppm.h
	$(CC) $(CFLAGS) -c $(PPM) -o $@ -lm
	
steg-decode.o: $(STEG-DECODE) bit-array.h error.h ppm.h eratosthenes.h
	$(CC) $(CFLAGS) -c $(STEG-DECODE) -o $@ -lm

# Spustitelny soubor pro prvocisla
prvocisla: prvocisla.o error.o eratosthenes.o
	$(CC) $(CFLAGS) prvocisla.o error.o eratosthenes.o -o $@ -lm

# Spustitelny soubor pro prvocisla-inline
prvocisla-inline: prvocisla.c error.o eratosthenes.c
	$(CC) $(CFLAGS) -DUSE_INLINE prvocisla.c error.o eratosthenes.c -o $@ -lm

# Spustitelny soubor pro steg-decode
steg-decode: error.o eratosthenes.o ppm.o steg-decode.o
	$(CC) $(CFLAGS) error.o eratosthenes.o ppm.o steg-decode.o -o $@ -lm
