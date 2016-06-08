#
# Makefile k implementaci projektu do predmetu IMS.
# Autor: Vojtech Havlena (xhavle03), Karel Brezina (xbrezi13)
#

all: simul.cpp state.o gasPipeline.o lngTerminal.o
	g++ simul.cpp state.o gasPipeline.o lngTerminal.o -o simul -std=c++11

state.o: state.cpp state.h
	g++ -c state.cpp -o state.o -std=c++11

gasPipeline.o: gasPipeline.cpp gasPipeline.h
	g++ -c gasPipeline.cpp -o gasPipeline.o -std=c++11

lngTerminal.o: lngTerminal.cpp lngTerminal.h
	g++ -c lngTerminal.cpp -o lngTerminal.o -std=c++11

run:
	./simul 3

clean:
	rm  -f *.o
