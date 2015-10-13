#!/bin/bash

for i in {1..5}
do
    bin/clear_cache.sh
    bin/disk_test data/1GB.dat 1GB_out.dat
done;