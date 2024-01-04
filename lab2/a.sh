#!/bin/bash
curl -JL -o demo.bin https://inp.zoolab.org/binflag/challenge\?id\=110652034

gcc lab2.c
output=$(./a.out)

response=$(curl -s "https://inp.zoolab.org/binflag/verify?v=$output")
echo "Response from the server:"
echo "$response"