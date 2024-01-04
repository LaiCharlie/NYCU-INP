#!/bin/bash

# Run the first shell script in the background
./b.sh &

# Run the second shell script in the background
./a.sh &

# Optionally, wait for both scripts to finish
wait

python3 verfy.py