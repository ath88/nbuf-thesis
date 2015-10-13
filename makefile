CC=g++

CFLAGS=-Wall
LIBS=-pthread --std=c++11

all: examples
test: test_data test_scripts


examples: bin/minmaxavgdisk bin/minmaxavgmemory

bin/minmaxavgdisk: bin object/nbuf.o
	$(CC) $(CFLAGS) $(LIBS) object/nbuf.o src/minmaxavgdisk.cpp -o $@

bin/minmaxavgmemory: bin object/nbuf.o
	$(CC) $(CFLAGS) $(LIBS) object/nbuf.o src/minmaxavgmemory.cpp -o $@

bin:
	mkdir $@


object/nbuf.o: object
	$(CC) $(CFLAGS) $(LIBS) src/nbuf.cpp -c -o $@

object:
	mkdir $@



test_scripts: bin/sequential_minmaxavg bin/disk_test bin/memory_test

bin/sequential_minmaxavg: bin
	$(CC) $(CFLAGS) $(LIBS) src/sequential_minmaxavg.cpp -o $@

bin/disk_test: bin
	$(CC) $(CFLAGS) $(LIBS) src/disk_test.cpp -o $@

bin/memory_test: bin
	$(CC) $(CFLAGS) $(LIBS) src/memory_test.cpp -o $@



test_data: data/1GB.dat

data/1GB.dat: data
	script/generate_data.pl 1GB

data:
	mkdir $@


clean:
	rm -rf object
	rm -rf data
	rm -rf bin