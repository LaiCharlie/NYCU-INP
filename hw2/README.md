# INP112 Homework 02

**Date:** 2023-12-21

## Description

In this homework, you are asked to design a chat server. Your program should be a single process and use select or poll to handle multiple connections and receive user commands from the client socket. After receiving the command message, the server should send the corresponding message back.

The TAs use the diff tool to compare your output directly against our prepared sample test data. When comparing against the sample test data, continuous spaces and tabs in the output are consolidated into a single space character.

Your server should show a welcome message once a client is connected.

```plaintext
*********************************
** Welcome to the Chat server. **
*********************************
```
# Basic Commands
The server accepts commands sent from the users, processes the commands, and responds to the users. The details of valid commands are introduced as follows.

If a client sends an incomplete command, e.g., missing required parameters, the server should show the client the correct command format (usage).

### Register
```plaintext
register <username> <password>
```

If there is any missing or redundant parameter:
Usage: register <username> <password>

If <username> already exists:
Username is already used.

If a client successfully registers an account:
Register successfully.

:bulb: The username and password only contain one word. Their respective length is less than 20 characters.

### Login
```plaintext
login <username> <password>
```
If there is any missing or redundant parameter:
Usage: login <username> <password>

If the client has already logged in with an account:
Please logout first.

If the username does not exist:
Login failed.

If the password is wrong:
Login failed.

If an account is already logged in by another client:
Login failed.

If a client successfully logs in:
Welcome, <username>.

:bulb: If client A has already successfully logged in as user A, other clients (including client A) cannot log in as user A until client A logs out.

### logout
```plaintext
logout
```
If there is any missing or redundant parameter
Usage: logout
If the client has not logged in
Please login first.
If the client successfully logs out
Bye, <username>.
exit

### exit
```plaintext
exit
```
If there is any missing or redundant parameter
Usage: exit
If the client has not logged out, run the logout command first and then the exit command implicitly. The terminal would show:
Bye, <username>.
If the exit command works successfully, the connection will be closed.

### whoami
```plaintext
whoami
```
If there is any missing or redundant parameter:
Usage: whoami
If the client has not logged in:
Please login first.
Show the username:
<username>

### set-status

```plaintext
set-status <status>
```
<status>: online, offline, busy

If there is any missing or redundant parameter:
Usage: set-status <status>
If the client has not logged in:
Please login first.
The status should be online, offline, or busy. If there is any other status:
set-status failed
The default status for all users is offline:
If setting successfully:
<username> <status>

:bulb: The status of a user should be set to online after login. The status of a user should be set to offline after logging out or exiting. You do not need to show any set-status-related messages when invoking login, logout, and exit commands.

### list-user
```plaintext
list-user
```

If there is any missing or redundant parameter:
Usage: list-user
If the client has not logged in:
Please login first.
List all users and the corresponding status in the server and sort by user names alphabetically.
<username 1> <status>
<username 2> <status>
...

### enter-chat-room
```plaintext
enter-chat-room <number>
```

If there is any missing or redundant parameter:
Usage: enter-chat-room <number>
If <number> is not a valid room number (not between 1 to 100)
Number <number> is not valid.
If the client has not logged in:
Please login first.
If the chat room does not exist, create a new room and enter this room:
Welcome to the public chat room.
Room number: <number>
Owner: <creator>
If one client successfully enters the chat room:
Show the message to all clients in the chat room.
<username> had entered the chat room.
Show the messages below to the new client.
Welcome to the public chat room.
Room number: <number>
Owner: <creator>
<chat_history>
Format of <chat_history>:
Chat history is composed of several Records.
Record 1 + Record 2 + … + Record n
The format of a Record is
[<username>]: <message>\n
Only show the latest 10 Records.

:bulb: Every <message> in <chat_history> should also be filtered (see here).

### unknown command

Show the error message if the input command is not specified:
Error: Unknown command

## Chat room commands
### pin
```plaintext
/pin <message>
```
Messages should be sent by TCP packets.
The length of the message can be at most 150 characters.
Send Pin -> [<username>]: <message>\n to all users in the chatroom.
Set the pin message in the chat room.

:bulb: Only one message can be pinned in a chat room. The chat room only keeps the latest pinned message.

### delete pin
```plaintext
/delete-pin
```
If there is no pin message in the chat room:
No pin message in chat room <number>
Delete the pin message in the chat room.

### exit chat room
```plaintext
/exit-chat-room
```
Switch to the chat server mode.
Send the message <username> had left the chat room. to all clients in the chat room.

### list user
```plaintext
/list-user
```
List all users in this chat room
<username> <status>
<username> <status>
...

### chat message
```plaintext
<message>
```
The length of the message can be at most 150 characters.
Send [<username>]: <message>\n to all users in the chatroom.

:bulb: In our test cases, <message> contains all printable ASCII characters except \0 and \n.

```plaintext
A client can use only chat room commands when entering a chat room. The client cannot use all basic commands in a chat room. Input without a leading / is considered a typical message. If a command is not specified in this document, the terminal will show Error: Unknown command .
```

## Chat History
When a client joins a (enter-chat-room) chat room, the server should immediately encapsulate all messages in chat history in the format of <chat_history> and send it to the client in the TCP connection. The format of <chat_history> is:

```plaintext
[<username>]: <message>\n[<username>]: <message>\n...
```

### Content Filtering
To keep our chat room a joyful and peaceful space, some keywords should not be used. Below is the filtering list of those keywords. The server must mask each matched word in <message> sent by the clients by replacing the word with a sequence of * characters of the same length.

Filtering List:

```plaintext
Copy code
Superpie
hello
Starburst Stream
Domain Expansion
```
:bulb: Keyword matching is case insensitive.

For more details about the implementation, please check the demonstration section for the sample input and the corresponding output.

Use ‘’% “ as the basic command line prompt. Notice that there is only one space after the prompt message. The server closes the connection for the client invoking the exit command. Nevertheless, the server keeps running in the background and accepts new incoming clients.

To run your server, you must provide the port number for your program. You can use nc or telnet command line tools to connect to your server.
```plaintext
Server Usage: ./hw2_chat_server [port number]
```

# Demonstration
### Example 1

ta1
``` plaintext
bash$ nc localhost 1234
*********************************
** Welcome to the Chat server. **
*********************************
1. % register
   Usage: register <username> <password>

2. % register ta1 420420
   Register successfully.

3. % register ta1 420420
   Username is already used.

4. % login
   Usage: login <username> <password>

5. % login ta1
   Usage: login <username> <password>

6. % login ta1 000000
   Login failed.

7. % login Tom 420420
   Login failed.

8. % login ta1 420420
   Welcome, ta1.

9. % whoami
   ta1

10. % login ta1 420420
    Please logout first.

11. % logout
    Bye, ta1.

12. % logout
    Please login first.

13. % register ta2 777777
    Register successfully.

17. % exit
```

ta3 (login after ta2 has registered)
``` plaintext
bash$ nc localhost 1234
*********************************
** Welcome to the Chat server. **
*********************************

14. % register ta3 ta3
    Register successfully.

15. % login ta3 ta3
    Welcome, ta3.

16. % list-user
    ta1 offline
    ta2 offline
    ta3 online

18. % set-status happyhappy
    set-status failed

19. % set-status busy
    ta3 busy

20. % list-user
    ta1 offline
    ta2 offline
    ta3 busy

21. % exit
    Bye, ta3.
```


## Scoring
[20pts] Your server passes Example 1.
[20pts] Your server passes Example 2.
[20pts] Your server passes Example 3.
[40pts] Your server passes the hidden test cases announced by TAs on January 4th.
Homework Submission
We will compile your homework by simply typing make.

Please pack your C/C++ code into a zip archive. The directory structure must follow the below illustration.

Please note that you do not need to enclose your id with the braces.

You must put your files in a folder named {id}_hw2 before you compress the files.

If the file structure is wrong after decompression, you will get a 10 pts score penalty.
If we cannot use ./hw2_chat_server [port number] command to run after typing make, you will get a 10 pts score penalty.

```r
Copy code
{student_id}_hw2.zip
└── {student_id}_hw2/
    ├── hw2_chat_server.c/hw2_chat_server.cpp
    ├── Makefile
    └── (any other c/c++ files if needed)
```

## Remarks
Please implement your homework in C or C++.
Using any non-standard libraries or any external binaries (e.g., via system() or other functions that invoke an external process) is not allowed.
Please upload your homework to E3 before the deadline.
We will check your uploaded time to make sure late submission.
You will get 0 if you don’t demo on 1/4 or upload to E3 after 1/4 13:00.
