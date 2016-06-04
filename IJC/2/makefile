
#
# Makefile pro 2. domaci ukol do predmetu IJC
# Autor: Vojtech Havlena, xhavle03
#
# Pro spravne nacteni dynamicke knihovny je nutne pridat cestu ke
# knihovne do promenne LD_LIBRARY_PATH.
#

CC=gcc
CPARAMS=-std=c99 -Wall -pedantic
CPP=g++-4.7
CPPPARAMS=-std=c++11 -Wall -pedantic
LIBRARYOBJ=hash_function.o htable_statistics.o htable_free.o htable_clear.o htable_foreach.o htable_lookup.o htable_init.o htable_remove.o

all: wordcount wordcount-dynamic tail tail2

htable_init.o: htable.h htable_init.c
	$(CC) $(CPARAMS) -c -fPIC htable_init.c -o $@
	
htable_lookup.o: htable.h htable_lookup.c
	$(CC) $(CPARAMS) -c -fPIC htable_lookup.c -o $@

htable_foreach.o: htable.h htable_foreach.c
	$(CC) $(CPARAMS) -c -fPIC htable_foreach.c -o $@	

htable_remove.o: htable.h htable_remove.c
	$(CC) $(CPARAMS) -c -fPIC htable_remove.c -o $@	
	
htable_clear.o: htable.h htable_clear.c
	$(CC) $(CPARAMS) -c -fPIC htable_clear.c -o $@

htable_free.o: htable.h htable_free.c
	$(CC) $(CPARAMS) -c -fPIC htable_free.c -o $@

htable_statistics.o: htable.h htable_statistics.c
	$(CC) $(CPARAMS) -c -fPIC htable_statistics.c -o $@
	
hash_function.o: hash_function.h hash_function.c
	$(CC) $(CPARAMS) -c -fPIC hash_function.c -o $@
	
io.o: io.h io.c
	$(CC) $(CPARAMS) -c -fPIC io.c -o $@
	
#Staticka knihovna
libhtable.a: $(LIBRARYOBJ)
	ar rcs $@ $(LIBRARYOBJ)
	ranlib $@

#Dynamicka knihovna
libhtable.so: $(LIBRARYOBJ)
	$(CC) -shared -fPIC $(LIBRARYOBJ) -o $@

#Program wordcount sestaveny se statickou knihovnou
wordcount: htable.h wordcount.c io.o libhtable.a 
	$(CC) $(CPARAMS) -static wordcount.c io.o libhtable.a -o $@

#Program wordcount-dynamic sestaveny s dynamickou knihovnou
wordcount-dynamic: htable.h wordcount.c io.o libhtable.so
	$(CC) $(CPARAMS) wordcount.c io.o -o $@ -L. -lhtable

#Program tail (C)
tail: tail.c
	$(CC) $(CPARAMS) tail.c -o $@

#Program tail2 (C++)
tail2: tail2.cc
	$(CPP) $(CPPPARAMS) tail2.cc -o $@
	
clean:
	rm *.o libhtable.a libhtable.so
	
