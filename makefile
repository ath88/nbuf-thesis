CC=g++

OPTS=-Wall
LIBS=-pthread --std=c++11

all:
	$(CC) $(OPTS) $(LIBS) src/nbuf.cpp -c
	$(CC) $(OPTS) $(LIBS) nbuf.o src/minmaxavg.cpp -o bin/minmaxavg