INP112 Homework 02
==================

Date: 2023-12-21

*   [INP112 Homework 02](https://md.zoolab.org/s/qTnmBptXZ#INP112-Homework-02 "INP112 Homework 02")
    *   [Description](https://md.zoolab.org/s/qTnmBptXZ#Description "Description")
    *   [Basic commands](https://md.zoolab.org/s/qTnmBptXZ#Basic-commands "Basic commands")
    *   [Chat room commands](https://md.zoolab.org/s/qTnmBptXZ#Chat-room-commands "Chat room commands")
        *   [Chat History](https://md.zoolab.org/s/qTnmBptXZ#Chat-History "Chat History")
        *   [Content Filtering](https://md.zoolab.org/s/qTnmBptXZ#Content-Filtering "Content Filtering")
    *   [Demonstration](https://md.zoolab.org/s/qTnmBptXZ#Demonstration "Demonstration")
        *   [Example 1](https://md.zoolab.org/s/qTnmBptXZ#Example-1 "Example 1")
        *   [Example 2](https://md.zoolab.org/s/qTnmBptXZ#Example-2 "Example 2")
        *   [Example 3](https://md.zoolab.org/s/qTnmBptXZ#Example-3 "Example 3")
    *   [Scoring](https://md.zoolab.org/s/qTnmBptXZ#Scoring "Scoring")
    *   [Homework Submission](https://md.zoolab.org/s/qTnmBptXZ#Homework-Submission "Homework Submission")
    *   [Remarks](https://md.zoolab.org/s/qTnmBptXZ#Remarks "Remarks")

***

[](https://md.zoolab.org/s/qTnmBptXZ#Description "Description")Description
--------------------------------------------------------------------------

In this homework, you are asked to design a chat server. Your program should be a **single process** and use [select](https://man7.org/linux/man-pages/man2/select.2.html) or [poll](https://man7.org/linux/man-pages/man2/poll.2.html) to handle **multiple connections** and receive user commands from the client socket. After receiving the command message, the server should send the corresponding message back.

The TAs use the diff tool to compare your output directly against our prepared sample test data. When comparing against the sample test data, continuous spaces and tabs in the output are consolidated into a single space character.

Your server should show a welcome message once a client is connected.

    *********************************
    ** Welcome to the Chat server. **
    *********************************
    

[](https://md.zoolab.org/s/qTnmBptXZ#Basic-commands "Basic-commands")Basic commands
-----------------------------------------------------------------------------------

The server accepts commands sent from the users, processes the commands, and responds to the users. The details of valid commands are introduced as follows.

If a client sends an incomplete command, e.g., missing required parameters, the server should show the client the correct command format (usage).

*   register
    
    > register <username> <password>
    
    1.  If there is any missing or redundant parameter
        *   **`Usage: register <username> <password>`**
    2.  If `<username>` already exists
        *   **`Username is already used.`**
    3.  If a client successfully registers an account
        *   **`Register successfully.`**
    
      
    
    ![:bulb:](https://md.zoolab.org/build/emojify.js/dist/images/basic/bulb.png)The username and password only contain one word. Their respective length is less than 20 characters.
    
*   login
    
    > login <username> <password>
    
    1.  If there is any missing or redundant parameter
        *   **`Usage: login <username> <password>`**
    2.  If the client has already logged in with an account
        *   **`Please logout first.`**
    3.  If the username does not exist
        *   **`Login failed.`**
    4.  If the password is wrong
        *   **`Login failed.`**
    5.  If an account is already logged in by another client
        *   **`Login failed.`**
    6.  If a client successfully logs in
        *   **`Welcome, <username>.`**  
              
            
    
    ![:bulb:](https://md.zoolab.org/build/emojify.js/dist/images/basic/bulb.png) If client A has already successfully logged in as user A, other clients (including client A) cannot log in as user A until client A logs out.
    
*   logout
    
    > logout
    
    1.  If there is any missing or redundant parameter
        *   **`Usage: logout`**
    2.  If the client has not logged in
        *   **`Please login first.`**
    3.  If the client successfully logs out
        *   **`Bye, <username>.`**
*   exit
    
    > exit
    
    1.  If there is any missing or redundant parameter
        *   **`Usage: exit`**
    2.  If the client has not logged out, run the `logout` command first and then the `exit` command implicitly. The terminal would show:
        *   **`Bye, <username>.`**
    3.  If the exit command works successfully, the connection will be closed.
    
      
    
*   whoami
    
    > whoami
    
    1.  If there is any missing or redundant parameter:
        *   **`Usage: whoami`**
    2.  If the client has not logged in:
        *   **`Please login first.`**
    3.  Show the username:
        *   **`<username>`**
*   set-status
    
    > set-status <status>  
    > <status>: `online`, `offline`, `busy`
    
    1.  If there is any missing or redundant parameter:
        *   **`Usage: set-status <status>`**
    2.  If the client has not logged in:
        *   **`Please login first.`**
    3.  The status should be `online`, `offline`, or `busy`. If there is any other status:
        *   **`set-status failed`**
    4.  The default status for all users is `offline`:
    5.  If setting successfully:
        *   **`<username> <status>`**
    
      
    
    ![:bulb:](https://md.zoolab.org/build/emojify.js/dist/images/basic/bulb.png) The status of a user should be set to `online` after login. The status of a user should be set to `offline` after logging out or exiting. You do not need to show any set-status-related messages when invoking `login`, `logout`, and `exit` commands.
    
*   list-user
    
    > list-user
    
    1.  If there is any missing or redundant parameter:
        *   **`Usage: list-user`**
    2.  If the client has not logged in:
        *   **`Please login first.`**
    3.  List all users and the corresponding status in the server and sort by user names alphabetically.
        *   **`<username 1> <status>`**
        *   **`<username 2> <status>`**
        *   **`...`**
*   Enter a chat room
    
    > enter-chat-room <number>
    
    1.  If there is any missing or redundant parameter:
        *   **`Usage: enter-chat-room <number>`**
    2.  If `<number>` is not a valid room number(not between 1 to 100)
        *   **`Number <number> is not valid.`**
    3.  If the client has not logged in:
        *   **`Please login first.`**
    4.  If the chat room does not exist, create a new room and enter this room:
        *   **`Welcome to the public chat room.`**
        *   **`Room number: <number>`**
        *   **`Owner: <creator>`**
    5.  If one client successfully enters the chat room:
        *   Show the message to all clients in the chat room.
            *   **`<username> had enter the chat room.`**
        *   Show the messages below to the new client.
            *   **`Welcome to the public chat room.`**
            *   **`Room number: <number>`**
            *   **`Owner: <creator>`**
            *   **`<chat_history>`**
        *   Format of **<chat\_history>**:
            *   Chat history is composed of several **Record**s.
                *   `Record 1` + `Record 2` \+ … + `Record n`
            *   The format of a **Record** is
                *   **`[<username>]: <message>\n`**
            *   Only show the latest 10 Records.
    
      
    
    ![:bulb:](https://md.zoolab.org/build/emojify.js/dist/images/basic/bulb.png) Every **<message>** in **<chat\_history>** should also be filtered (see [here](https://md.zoolab.org/OFndeXQNQjGCj6USJlqStQ?both#Content-Filtering)). If there is a pin message, add a mark after `Record n` in the format **`Pin -> [<username>]: <message>\n`**.(see [here](https://md.zoolab.org/OFndeXQNQjGCj6USJlqStQ?both#Requirement-command-in-Chat-Room))
    
*   list chat room
    
    > list-chat-room
    
    1.  If there is any missing or redundant parameter:
        *   **`Usage: list-chat-room`**
    2.  If the client has not logged in:
        *   **`Please login first.`**
    3.  List all existing chat rooms and the corresponding owners in the server and sort by the room number.
        *   **`<owner username> <room number>`**
        *   **`...`**
*   close chat room
    
    > close-chat-room <number>
    
    1.  If there is any missing or redundant parameter:
        *   **`Usage: close-chat-room <number>`**
    2.  If the client has not logged in:
        *   **`Please login first.`**
    3.  If the user is not the owner:
        *   **`Only the owner can close this chat room.`**
    4.  If the chat room does not exist:
        *   **`Chat room <number> does not exist.`**
    5.  After closing successfully:
        *   **`Chat room <number> was closed.`**
    
      
    
    ![:bulb:](https://md.zoolab.org/build/emojify.js/dist/images/basic/bulb.png) All users in a chat room must be kicked out when the room no longer exists. Once a chat room is closed, the server should show a message in the format **`Chat room <number> was closed.`** to all users in the chat room.
    
*   unknown command
    
    *   Show the error message if the input command is not specified:
        *   **`Error: Unknown command`**

[](https://md.zoolab.org/s/qTnmBptXZ#Chat-room-commands "Chat-room-commands")Chat room commands
-----------------------------------------------------------------------------------------------

*   pin
    
    > /pin <message>
    
    1.  Messages should be sent by TCP packets.
    2.  The length of the message can be at most 150 characters.
    3.  Send `Pin -> [<username>]: <message>\n` to all users in the chatroom.
    4.  Set the pin message in the chat room.
    
      
    
    ![:bulb:](https://md.zoolab.org/build/emojify.js/dist/images/basic/bulb.png) Only one message can be pinned in a chat room. The chat room only keeps the latest pinned message.
    
*   delete pin
    
    > /delete-pin
    
    1.  If there is no pin message in the chat room:
        *   **`No pin message in chat room <number>`**
    2.  Delete the pin message in the chat room.
*   exit chat room
    
    > /exit-chat-room
    
    1.  Switch to the chat server mode.
    2.  Send the message `<username> had left the chat room.` to all clients in the chat room.
*   list user
    
    > /list-user
    
    1.  List all users in this chat room
        *   **`<usename> <status>`**
        *   **`<usename> <status>`**
        *   **`...`**
*   chat message
    
    > <message>
    
    1.  The length of the message can be at most 150 characters.
    2.  Send `[<username>]: <message>\n` to all users in the chatroom.
    
      
    
    ![:bulb:](https://md.zoolab.org/build/emojify.js/dist/images/basic/bulb.png) In our test cases, **<message>** contains all printable ASCII characters except `\0` and `\n`.
    

A client can use only chat room commands when entering a chat room. The client cannot use all basic commands in a chat room. Input without a leading `/` is considered a typical **message**. If a command is not specified in this document, the terminal will show `Error: Unknown command` .

### [](https://md.zoolab.org/s/qTnmBptXZ#Chat-History "Chat-History")Chat History

When a client joins a (`enter-chat-room`) chat room, the server should immediately encapsulate all messages in **chat history** in **`the format of <chat_history>`** and send it to the client in the **TCP connection**. The **`format of <chat_history>`** is:

    [<username>]: <message>\n[<username>]: <message>\n...
    

### [](https://md.zoolab.org/s/qTnmBptXZ#Content-Filtering "Content-Filtering")Content Filtering

To keep our chat room a joyful and peaceful space, some keywords should not be used. Below is the filtering list of those keywords. The server must mask each matched word in `<message>` sent by the clients by replacing the word with a sequence of `*` characters of the same length.

**You must hard code the filtering list in your C/C++ program file.**

#### [](https://md.zoolab.org/s/qTnmBptXZ#filtering-list "filtering-list")filtering list

    
    
    
    
    ==
    Superpie
    hello
    Starburst Stream
    Domain Expansion
    

![:bulb:](https://md.zoolab.org/build/emojify.js/dist/images/basic/bulb.png) Keyword matching is **case insensitive**.

For more details about the implementation, please check the demonstration section for the sample input and the corresponding output.

Use ‘’% “ as the basic command line prompt. Notice that there is only one space after the prompt message. The server closes the connection for the client invoking the exit command. Nevertheless, the server keeps running in the background and accepts new incoming clients.

To run your server, you must provide the port number for your program. You can use nc or telnet command line tool to connect to your server.

    server usage: ./hw2_chat_server [port number]
    

[](https://md.zoolab.org/s/qTnmBptXZ#Demonstration "Demonstration")Demonstration
--------------------------------------------------------------------------------

### [](https://md.zoolab.org/s/qTnmBptXZ#Example-1 "Example-1")Example 1

> `register` `login` `logout` `whoami` `set-status` `list-user`

*   ta1

        bash$ nc localhost 1234
        *********************************
        ** Welcome to the Chat server. **
        *********************************
    1.  % register
        Usage: register <username> <password>
    2.  % register ta1 420420
        Register successfully.
    3.  % register ta1 420420
        Username is already used.
    4.  % login
        Usage: login <username> <password>
    5.  % login ta1
        Usage: login <username> <password>
    6.  % login ta1 000000
        Login failed.
    7.  % login Tom 420420
        Login failed.
    8.  % login ta1 420420
        Welcome, ta1.
    9.  % whoami
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
    

*   ta3 (login after ta2 has registered)

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
    

### [](https://md.zoolab.org/s/qTnmBptXZ#Example-2 "Example-2")Example 2

> `enter-chat-room <number>` `list-chat-room` `close-chat-room <number>` `unknown command` `chat`

*   ta1 (login as a registered user ta1)

        *********************************
        ** Welcome to the Chat server. **
        *********************************
    1.  % login ta1 420420
        Welcome, ta1.
    2.  % enter-chat-room
        Usage: enter-chat-room <number>
    3.  % enter-chat-room 1010
        Number 1010 is not valid.
    4.  % enter-chat-room 20
        Welcome to the public chat room.
        Room number: 20
        Owner: ta1
    5.  /exit-chat-room
    6.  % enter-chat-room 30
        Welcome to the public chat room.
        Room number: 30
        Owner: ta1
    7.  hello, i am ta1.
        [ta1]: *****, i am ta1.
    8.  i am waiting for everyone.
        [ta1]: i am waiting for everyone.
        ta2 had enter the chat room.
        [ta2]: merry christmas!
    15. exit
        [ta1]: exit
    17. /exit-chat-room
    18. % list-chat-room
        ta1 20
        ta1 30
    19. % close-chat-room 35
        Chat room 35 does not exist.
    20. % close-chat-room 20
        Chat room 20 was closed.
    21. % list-chat-room
        ta1 30
    22. % close-chat-room 30
        Chat room 30 was closed.
    23. % list-chat-room
    24. % exit
        Bye, ta1.
    

*   ta2 (login as a registered user ta2)

        *********************************
        ** Welcome to the Chat server. **
        *********************************
    9.  % login ta2 ta2
        Welcome, ta2.
    10. % list-chat-room
        ta1 20
        ta1 30
    11. % close-chat-room
        Usage: close-chat-room <number>
    12. % close-chat-room 30
        Only the owner can close this chat room.
    13. % enter-chat-room 30
        Welcome to the public chat room.
        Room number: 30
        Owner: ta1
        [ta1]: *****, i am ta1.
        [ta1]: i am waiting for everyone.
    14. merry christmas!
        [ta2]: merry christmas!
        [ta1]: exit
    16. /close-chat-room 30
        Error: Unknown command
        ta1 had left the chat room.
        Chat room 30 was closed.
    25. % close-chat-room 30
        Chat room 30 does not exist.
    26. % hello
        Error: Unknown command
    27. % exit
        Bye, ta2.
    

### [](https://md.zoolab.org/s/qTnmBptXZ#Example-3 "Example-3")Example 3

> `/pin <message>` `/delete-pin` `/exit-chat-room` `/list-user` `chat`

*   Bob (The second `enter-chat-room` command is sent after Tom enters the chat room)

        *********************************
        ** Welcome to the Chat server. **
        *********************************
    1.  % register Bob 55555
        Register successfully.
    2.  % login Bob 55555
        Welcome, Bob. 
    3.  % enter-chat-room 25
        Welcome to the public chat room.
        Room number: 25
        Owner: Bob
    4.  I will win !!!
        [Bob]: I will win !!!
    5.  /delete-pin
        No pin message in chat room 25
    6.  /pin You are the challenger.
        Pin -> [Bob]: You are the challenger.
    7.  /exit-chat-room   
    11. % enter-chat-room 25
        Welcome to the public chat room.
        Room number: 25
        Owner: Bob
        [Bob]: I will win !!!
        Pin -> [Bob]: You are the challenger.
        [Tom]: *****
    13. ?
        [Bob]: ?
    14. domain expansion.
        [Bob]: ****************.
        [Tom]: What?
    16. domain expansion!!!???
        [Bob]: ****************!!!???
        Pin -> [Tom]: You are an ordinary person.
    18. /exit-chat-room
    19. % set-status offline
        Bob offline
    20. % enter-chat-room 25
        Welcome to the public chat room.
        Room number: 25
        Owner: Bob
        [Bob]: I will win !!!
        [Tom]: *****
        [Bob]: ?
        [Bob]: ****************.
        [Tom]: What?
        [Bob]: ****************!!!???
        Pin -> [Tom]: You are an ordinary person.
    21. I'm sorry.
        [Bob]: I'm sorry.
    22. I couldn't bring out the best in you.
        [Bob]: I couldn't bring out the best in you.
    24. /delete-pin
        [Tom]: I won't forget you.
        [Tom]: You can't beat me.
        [Tom]: Cheer up !!!
    28. /exit-chat-room
    % 
    

*   Tom (enter the chat room after the first `exit-chat-room` command of Bob)

        *********************************
        ** Welcome to the Chat server. **
        *********************************
    8.  % register Tom 22222
        Register successfully.
    9.  % login Tom 22222
        Welcome, Tom.
    10. % enter-chat-room 25
        Welcome to the public chat room.
        Room number: 25
        Owner: Bob
        [Bob]: I will win !!!
        Pin -> [Bob]: You are the challenger.
        Bob had enter the chat room.
    12. hello
        [Tom]: *****
        [Bob]: ?
        [Bob]: ****************.
    15. What?
        [Tom]: What?
        [Bob]: ****************!!!???
    17. /pin You are an ordinary person.
        Pin -> [Tom]: You are an ordinary person.
        Bob had left the chat room.
        Bob had enter the chat room.
        [Bob]: I'm sorry.
        [Bob]: I couldn't bring out the best in you.
    23. /list-user
        Bob offline
        Tom online
    25. I won't forget you.
        [Tom]: I won't forget you.
    26. You can't beat me.
        [Tom]: You can't beat me.
    27. Cheer up !!!
        [Tom]: Cheer up !!!
        Bob had left the chat room.
    29. Bye Bye.
        [Tom]: Bye Bye.
    30. Who else?
        [Tom]: Who else? 
        nobody had enter the chat room.
        nobody had left the chat room.
    35. /exit-chat-room
    % 
    

*   nobody (enter the chat room after the third `exit-chat-room` command of Bob)

        *********************************
        ** Welcome to the Chat server. **
        *********************************
    31. % register nobody 11111
        Register successfully.
    32. % login nobody 11111
        Welcome, nobody.
    33. % enter-chat-room 25
        Welcome to the public chat room.
        Room number: 25
        Owner: Bob
        [Bob]: ****************.
        [Tom]: What?
        [Bob]: ****************!!!???
        [Bob]: I'm sorry.
        [Bob]: I couldn't bring out the best in you.
        [Tom]: I won't forget you.
        [Tom]: You can't beat me.
        [Tom]: Cheer up !!!
        [Tom]: Bye Bye.
        [Tom]: Who else?
    34. /exit-chat-room
    % 
    

[](https://md.zoolab.org/s/qTnmBptXZ#Scoring "Scoring")Scoring
--------------------------------------------------------------

1.  \[20pts\] Your server passes Example 1.
2.  \[20pts\] Your server passes Example 2.
3.  \[20pts\] Your server passes Example 3.
4.  \[40pts\] Your server passes the hidden test cases announced by TAs on January 4th.

[](https://md.zoolab.org/s/qTnmBptXZ#Homework-Submission "Homework-Submission")Homework Submission
--------------------------------------------------------------------------------------------------

We will compile your homework by simply typing `make`.

Please pack your C/C++ code into a zip archive. The directory structure must follow the below illustration.

Please note that you do not need to enclose your id with the braces.

You must put your files in a folder named `{id}_hw2` before you compress the files.

If the file structure is wrong after decompression, you will get 10 pts score penalty.  
If we cannot use `./hw2_chat_server [port number]` command to run after typing make, you will get 10 pts score penalty.

    {student_id}_hw2.zip
    └── {student_id}_hw2/
        ├── hw2_chat_server.c/hw2_chat_server.cpp
        ├── Makefile
        └── (any other c/c++ files if needed) 
    

[](https://md.zoolab.org/s/qTnmBptXZ#Remarks "Remarks")Remarks
--------------------------------------------------------------

*   Please implement your homework in C or C++.
*   Using any non-standard libraries or any external binaries (e.g., via system() or other functions that invoke an external process) is not allowed.

Please upload your homework to E3 before the deadline.  
We will check your uploaded time to make sure late submission.  
**You will get 0 if you don’t demo on 1/4 or upload to E3 after 1/4 13:00.**
