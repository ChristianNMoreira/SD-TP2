#!/bin/bash
gcc -Wall -Werror -D_POSIX_THREAD_SEMANTICS -o prodCons prodCons.c -pthread

for N in 1 10 100 1000
    do
    ./prodCons $N 1 1
    for i in 2, 4, 8
    do
        ./prodCons $N 1 $i
        ./prodCons $N $i 1
    done
done