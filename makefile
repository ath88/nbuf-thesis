CC=g++

OPTS=-Wall
LIBS=-pthread --std=c++11

all:
	$(CC) $(OPTS) $(LIBS) src/nbuf.cpp -c
	$(CC) $(OPTS) $(LIBS) nbuf.o src/minmaxavgdisk.cpp -o bin/minmaxavgdisk
	$(CC) $(OPTS) $(LIBS) nbuf.o src/minmaxavgmemory.cpp -o bin/minmaxavgmemory

test_data:
	data/generate_data.pl 1b
	data/generate_data.pl 1KB
	data/generate_data.pl 1MB
	data/generate_data.pl 1GB
	data/generate_data.pl 10GB

clean:
	rm data/*.dat
	rm bin/*