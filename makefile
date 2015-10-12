CC=g++

OPTS=-Wall
LIBS=-pthread --std=c++11

all:
	$(CC) $(OPTS) $(LIBS) src/nbuf.cpp -c
	$(CC) $(OPTS) $(LIBS) nbuf.o src/minmaxavgdisk.cpp -o bin/minmaxavgdisk
	$(CC) $(OPTS) $(LIBS) nbuf.o src/minmaxavgmemory.cpp -o bin/minmaxavgmemory

tests:
	$(CC) $(OPTS) $(LIBS) src/seq_minmaxavg.cpp -o bin/seq_minmaxavg
	$(CC) $(OPTS) $(LIBS) src/disk_test.cpp -o bin/disk_test
	$(CC) $(OPTS) $(LIBS) src/memory_test.cpp -o bin/memory_test

test_data:
	data/generate_data.pl 1b
	data/generate_data.pl 1KB
	data/generate_data.pl 1MB
	data/generate_data.pl 1GB
	data/generate_data.pl 10GB

clean:
	rm data/*.dat
	rm bin/*