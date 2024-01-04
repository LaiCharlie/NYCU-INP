INP112 Lab03
============

Date: 2023-10-19

*   [INP112 Lab03](https://md.zoolab.org/s/5cxr0vEHM#INP112-Lab03 "INP112 Lab03")
*   [Play with Mazes](https://md.zoolab.org/s/5cxr0vEHM#Play-with-Mazes "Play with Mazes")
    *   [Description](https://md.zoolab.org/s/5cxr0vEHM#Description "Description")
    *   [Demonstration](https://md.zoolab.org/s/5cxr0vEHM#Demonstration "Demonstration")

[](https://md.zoolab.org/s/5cxr0vEHM#Play-with-Mazes "Play-with-Mazes")Play with Mazes
======================================================================================

[](https://md.zoolab.org/s/5cxr0vEHM#Description "Description")Description
--------------------------------------------------------------------------

This lab aims to practice implementing clients to interact with TCP servers. We prepared four different flavors of text-based maze servers. Please follow the instructions and have fun!

1.  You may play with the maze servers _manually_ before implementing the clients. The servers can be accessed via the following commands:
    
        nc inp.zoolab.org 10301
        nc inp.zoolab.org 10302
        nc inp.zoolab.org 10303
        nc inp.zoolab.org 10304
        
    
2.  Each server displays a full or partial maze upon connecting to the server. You (or your client implementation) have to walk from the current position (indicated as an asterisk `*`) to the exit `E`. The sharp (`#`) character indicates a wall in the maze, and the dot (`.`) character indicates a road in the maze.
    
3.  To change the current position, you can send commands `W`, `S`, `A`, and `D` to move up, down, left, and right, respectively.
    
4.  The time limits for solving the mazes are as follows:
    
    *   maze #1: 60s
    *   maze #2: 60s
    *   maze #3: 60s
    *   maze #4: 600s
5.  If you have correctly solved a maze, i.e., walk from the initial position to the exit `E`, the server displays a `Bingo!` message followed by a secret message. Please paste the secret message to the verification service running at [https://inp.zoolab.org/maze/service](https://inp.zoolab.org/maze/service), and then the TAs can verify if your solution is correct.
    

[](https://md.zoolab.org/s/5cxr0vEHM#Demonstration "Demonstration")Demonstration
--------------------------------------------------------------------------------

1.  \[20%\] Your implemented TCP client can solve maze #1. Repeat running your solver program three times. Each time, your solver must solve the challenge successfully.
    
2.  \[25%\] Your implemented TCP client can solve maze #2. Repeat running your solver program three times. Each time, your solver must solve the challenge successfully.
    
3.  \[30%\] Your implemented TCP client can solve maze #3. Repeat running your solver program three times. Each time, your solver must solve the challenge successfully.
    
4.  \[25%\] Your implemented TCP client can solve maze #4. Repeat running your solver program three times. Your solver should find a correct solution at least one time.
