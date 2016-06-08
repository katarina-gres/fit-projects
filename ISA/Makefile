
#
# Makefile ircbota pro projekt do predmetu ISA
# Autor: Vojtech Havlena, xhavle03
# Datum: 7. 10. 2014
#

CPPFLAGS=-std=c++11 -Wall -pedantic

all: IrcParser.o SyslogHandler.o ircbot.h
	g++ $(CPPFLAGS) ircbot.cpp IrcParser.o SyslogHandler.o -o ircbot

IrcParser: IrcParser.h
	g++ $(CPPFLAGS) -c IrcParser.cpp -o IrcParser.o

SyslogHandler: SyslogHandler.h
	g++ $(CPPFLAGS) -c SyslogHandler.cpp -o SyslogHandler.o

clean:
	rm  -f *.o
