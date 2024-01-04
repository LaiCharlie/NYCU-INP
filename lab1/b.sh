#!/bin/bash

# Start tcpdump to capture packets
sudo tcpdump -ni any -Xxnv udp and port 10495 -w try.pcap &

# Sleep for 10 seconds
sleep 20

# Stop tcpdump
sudo pkill tcpdump

# Run lab1.py
python3 lab1.py