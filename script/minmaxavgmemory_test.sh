#!/bin/bash

for seq_ex in f t
do
#    echo $seq_ex
    for memory in 1 10 100 1000
    do
#        echo $memory
        for thread in 1 2 4 8 16 32 64 128
        do
#            echo $thread
            for i in {1..5}
            do
                echo $thread $memory $seq_ex
                if [ $thread == 2 ]; then
                    taskset 3 bin/minmaxavgmemory $thread $memory $seq_ex
                else
                    bin/minmaxavgmemory $thread $memory $seq_ex
                fi
            done;
        done;
    done;
done;