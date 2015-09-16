CC=g++

LIBS=-pthread --std=c++11

all:
	$(CC) $(LIBS) code/block.cpp -o bin/block
	$(CC) $(LIBS) code/object.cpp -o bin/object