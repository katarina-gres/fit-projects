# Projekt: Implementace imperativniho jazyka IFJ13
# Autori: xbrezi13, Karel Brezina
#         xhavle03, Vojtech Havlena
#         xdobro12, Matus Dobrotka
#         xraszk03, Ales Raszka
#         xjerab18, Kamil Jerabek

######################################################################
## Nazev: Makefile 
## Popis: Preklad zdrojovych souboru
######################################################################

OPTION=-std=c99 -Wall -Wextra -pedantic
PROGS= main.o adt.o built_in.o expr.o ext_string.o generator.o ial.o instruction.o interpret.o lex.o parser.o semantic.o forstack.o

all: test clear 

test: ${PROGS}
	gcc ${OPTION} ${PROGS} -o ./ifj13 -lm

main.o: main.c global.h instruction.h interpret.h parser.h adt.h ial.h
	gcc ${OPTION} -c main.c -o main.o

adt.o: adt.c adt.h
	gcc ${OPTION} -c adt.c -o adt.o

built_in.o: built_in.c built_in.h
	gcc ${OPTION} -c built_in.c -o built_in.o -lm

expr.o: expr.c parser.h ial.h expr.h adt.h generator.h 
	gcc ${OPTION} -c expr.c -o expr.o

ext_string.o: ext_string.c ext_string.h
	gcc ${OPTION} -c ext_string.c -o ext_string.o

generator.o: generator.c generator.h
	gcc ${OPTION} -c generator.c -o generator.o

ial.o: ial.c ial.h
	gcc ${OPTION} -c ial.c -o ial.o

instruction.o: instruction.c instruction.h
	gcc ${OPTION} -c instruction.c -o instruction.o

interpret.o: interpret.c interpret.h built_in.h keywords.h adt.h ial.h
	gcc ${OPTION} -c interpret.c -o interpret.o

lex.o: lex.c lex.h ext_string.h ial.h adt.h keywords.h
	gcc ${OPTION} -c lex.c -o lex.o
	
forstack.o: forstack.c forstack.h expr.h ial.h
	gcc ${OPTION} -c forstack.c -o forstack.o

parser.o: parser.c parser.h lex.h keywords.h expr.h generator.h semantic.h interpret.h global.h instruction.h forstack.h
	gcc ${OPTION} -c parser.c -o parser.o

semantic.o: semantic.c semantic.h parser.h
	gcc ${OPTION} -c semantic.c -o semantic.o

clear:
	rm -f *.o 
