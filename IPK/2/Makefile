#
# Makefile pro 2. projekt do predmetu IPK.
# Autor: Vojtech Havlena, xhavle03
#

CPPFLAGS=-std=c++11 -pthread -Wall 

all: server client 

server: server.cc 
	g++48 $(CPPFLAGS) server.cc -o $@ -static-libstdc++ 

client: client.cc 
	g++48 $(CPPFLAGS) client.cc -o $@ -static-libstdc++

clean: 
	rm client server
