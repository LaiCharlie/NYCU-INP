INP112 Lab01
Date: 2023-09-28

INP112 Lab01
#1 Play with Docker
Description
Demonstration
#2 Simple Packet Analysis
Description
Demonstration
Network Issues
#1 Play with Docker
This lab aims to familiarize the students with docker and docker-compose commands. Please follow the instructions to complete this lab. Once you have completed everything, please demo your implementation to a TA.

You can do everything on your laptop or the classroom desktop PC.

Description
Please follow the steps listed below to complete this lab.

Prepare your own docker environment. You can install Docker Desktop on your laptop or simply use the “docker.io” package pre-installed in the classroom desktop PC.

Download the docker-compose.yml and Dockerfile from the course website.

For Apple Chip Users (M1/M2): You have to enable “Use Docker Compose V2” in your Docker Desktop options.

Build your docker container environment. Ensure that you have correctly set up your username and created the home directory for the user.

Follow the instructions in the introduction slide, compile textbook samples, and run in your container instance.

Demonstration
[20%] Show your user name and the user’s home directory

[20%] Ensure you have a working directory accessible from both the host and the docker runtime.

[20%] Rebuild the docker by removing the built one with docker-compose rm followed by docker-compose up -d --build. You don’t have to remove existing docker images and caches, so the rebuild process should be very fast.

[20%] Run the daytimetcpsrv and daytimetcpcli samples in your docker instance.

[20%] Change the port number used in daytimetcpsrv and daytimetcpcli, then test the server and client again.

For Apple Chip Users (M1/M2) You need to pass an additional argument --build=arm-linux to the configure command to ensure it can correctly detect your build environment.

#2 Simple Packet Analysis
This lab aims to practice simple packet analysis using tcpdump (or wireshark). You have to interact with the server and get a secret flag sent from the server. Once you obtain the secret flag from the server, you can then ask the server to verify the correctness of the flag.

Most UNIX operating systems have packages for tcpdump and wireshark. Nevertheless, wireshark can be downloaded from its official site.

Description
Please follow the steps listed below to complete this lab.

Run tcpdump (or wireshark) in your preferred environment.
Connect to the challenge server by running nc -u inp.zoolab.org 10495 from your preferred environment. This step can be done either inside or outside a docker.

Request a challenge id from the server by sending the command hello <id>, where <id> is your user id. Suppose a given <id> is chuang. You should receive an OK response from the server along with a challenge id composed of your id and a random string. chuang_d59acd9fb5577b76f0a82990c815eed8

Request a challenge using the obtained challenge id. You have to send a command chals <chals_id>, where the <chals_id> is the challenge id you received from the previous command.

The server sends many UDP packets, and each payload is in the form of SEQ/NNNNN:message. The NNNNN is a sequence number (starting from zero) for you to determine if there is any packet loss. The message can be one of the following:

An arbitrary length of character A's.
The BEGIN FLAG message
The END FLAG message
You can then get the flag of this challenge by following the steps:

Recognize the packet having the payload BEGIN FLAG. Once you have recognized the packet, perform flag extraction using the next few steps.
For each packet, obtain its x-header-length by summing up the length of the IP options and UDP payloads. Suppose 10 packets were received after the BEGIN FLAG packet. You should get 10 x-header-lengths.
Convert each x-header-length into its corresponding ASCII character.
The secret flag can be obtained by concatenating each ASCII character obtained in the second step in its order.
You can find the end of the flag by recognizing the END FLAG message in a packet’s UDP payload.
Decode the flag and verify if your flag is correct by connecting to the challenge server and using the verfy <flag> command to verify your decoded flag. The server responds GOOD JOB! if your flag is correct.

Demonstration
[50%] If you can implement a program to request the server to send challenges. Your program should be able to send different user id requested by the TAs.

[30%] If you can capture the challenge packets, save them in a pcap file, and decode a correct flag from the saved packets.

[20%] If your program can perform the above two steps in a single implementation (command) and display the decoded flag in the terminal.

For steps (2) and (3), the flag verification process can be performed manually.

A sample pcap file can be downloaded from here.

Your program may handle the outputs from the command tcpdump -ni any -Xxnv udp and port 10495

Note that your script (or program) can use a saved pcap file created by tcpdump or wireshark as its input.

Network Issues
If you have network connection issue with our challenge server, you can download the server binary from here, and run it in your Linux docker using the command

sudo ./udpflag_release 10495
You need root permission to run the server program.

Please note that you need to grant execution permission to the downloaded executable. If you are working with an Apple chip Mac, you have to install Rosetta (x86_64 emulation) to run this executable in your Linux docker.

If everything is fine, the server should only display a single message server: running on port 10495.

If you have successfully invoked the server, you should be able to connect to the server using the command

nc -u 127.0.0.1 10495
and the behavior should be the same as the remote challenge server.
