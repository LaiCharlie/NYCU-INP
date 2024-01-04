#!/bin/bash
g++ 4.cpp -o dfs.exe
for i in {1..3}
do
    ./dfs.exe > "demo${i}.txt" &
done
