INP112 Lab08
============

Date: 2023-12-14

*   [INP112 Lab08](https://md.zoolab.org/s/svtj6_CJN#INP112-Lab08 "INP112 Lab08")
*   [TUN-based VPN](https://md.zoolab.org/s/svtj6_CJN#TUN-based-VPN "TUN-based VPN")
    *   [The Challenge Server](https://md.zoolab.org/s/svtj6_CJN#The-Challenge-Server "The Challenge Server")
    *   [VPN Overview](https://md.zoolab.org/s/svtj6_CJN#VPN-Overview "VPN Overview")
    *   [Work with the Development Package](https://md.zoolab.org/s/svtj6_CJN#Work-with-the-Development-Package "Work with the Development Package")
    *   [Demonstration](https://md.zoolab.org/s/svtj6_CJN#Demonstration "Demonstration")

[](https://md.zoolab.org/s/svtj6_CJN#TUN-based-VPN "TUN-based-VPN")TUN-based VPN
================================================================================

Before starting to implement this lab, please ensure that you have basic command line tools installed and additional tools including `make`, `docker`, and `tmux` available in a UNIX-based runtime, e.g., native-Linux, native-MacOS with Docker Desktop, and Windows WSL2 with Docker Desktop integration. You may consider using our course VM-image if you cannot properly setup your runtime.

This lab aims to practice implementing a TUN-based VPN. You have to implement a server and a client. The server accepts connection requests from at least two clients. The server and the connected clients form a VPN network of 10.0.0.0/24, where the server always uses the IP address 10.0.0.254, and each of the connected clients uses a non-conflicting IP address assigned by the server (within the network of 10.0.0.0/24).

Due to the possible high complexity of this lab, the demo score weight is re-arranged as follows:

1.  week-1: 1.05 (in-class)
2.  week-2: 1.00
3.  week-3: 0.85
4.  week-4: 0.70

[](https://md.zoolab.org/s/svtj6_CJN#The-Challenge-Server "The-Challenge-Server")The Challenge Server
-----------------------------------------------------------------------------------------------------

There is no challenge server for this lab. You can implement everything with our prepared development package in your preferred **UNIX-based** runtime.

[](https://md.zoolab.org/s/svtj6_CJN#VPN-Overview "VPN-Overview")VPN Overview
-----------------------------------------------------------------------------

1.  A VPN network can be considered an **overlay network** built on top of an existing network. In our lab setting, a physical network `172.28.28.0/24` is created for direct communications between a server and clients in the network. We aim to build a VPN network `10.0.0.0/24` on top of the physical network. All the clients who join the VPN network can communicate with each other using the addresses in the `10.0.0.0/24` network. The concept of the overlay network is illustrated in the below figure.
    
    ![overlay](https://inp.zoolab.org/netprog/lab08/overlay.png)
    
2.  A VPN network can be implemented using the `tun` interface. Introductions of the `tun` interface can be found in [its official document](https://www.kernel.org/doc/Documentation/networking/tuntap.txt) or from [Wikipedia](https://en.wikipedia.org/wiki/TUN/TAP). `tun` is a virtual interface that allows a user-space process to handle packets sent to or received from the interface. Many VPN developers leverage the `tun` interface to implement their user-space VPN server and client programs, including the well-known [OpenVPN project](https://openvpn.net/community-resources/openvpn-project/). The component architecture of using `tun` interfaces to build a VPN network having one server and two clients is illustrated in the figure below.
    
    ![architecture](https://inp.zoolab.org/netprog/lab08/arch.png)
    
3.  Given the architecture above, the packet flow of the client1 pinging the server is annotated in the figure below. _**The orange line indicates the IP-in-IP (TCP or UDP) tunnel running your customized protocol.**_
    
    ![C1toS](https://inp.zoolab.org/netprog/lab08/arch-C1toS.png)
    
4.  With the same architecture, the packet flow of the client1 pinging the client2 is much more complicated, as annotated in the figure below. This is because all the VPN traffic must pass through the server. _**The orange line indicates the IP-in-IP (TCP or UDP) tunnel running your customized protocol.**_
    
    ![C1toC2](https://inp.zoolab.org/netprog/lab08/arch-C1toC2.png)
    
5.  It is worth mentioning that the `tun` interface supports two modes, i.e., the `tun` and the `tap` mode. The former supports the L3 (IP-based) tunnel, and the latter supports the L2 (Ethernet-based) tunnel. A user can choose the mode depending on the requirement of the VPN design.
    

[](https://md.zoolab.org/s/svtj6_CJN#Work-with-the-Development-Package "Work-with-the-Development-Package")Work with the Development Package
--------------------------------------------------------------------------------------------------------------------------------------------

Our runtime uses the network 172.28.28.0/24. In case it conflicts with your existing docker networks, you may try the following steps.

1.  Run `docker network prune` to remove unused networks.
2.  Modify `Makefile` to use a different network

1.  To get started, please download the [development package](https://inp.zoolab.org/netprog/lab08/lab_tun.tbz) prepared for this lab. Unpack the package, and there should be a `lab_tun` folder.
    
2.  Run `make build` in the `lab_tun` folder to build the required docker image to run and test your implementation. The command must be invoked at least once to build the image. Re-run it if the content of the `Dockerfile` is updated.
    
3.  A sample code is available in the `lab_tun/dist` folder. Run `make tunvpn` in the `lab_tun` folder to build the sample code. The source code is also available [here](https://inp.zoolab.org/netprog/lab08/tunvpn.cpp) ([view](https://inp.zoolab.org/code.html?file=netprog/lab08/tunvpn.cpp)). Several useful functions and constants can be found in the sample code. You should also check [netdevice(7)](https://man7.org/linux/man-pages/man7/netdevice.7.html) to see the data structure used for `ioctl` to control network interfaces.
    
    *   `int tun_alloc(char *dev)`: allocate a tun device. The size of `dev` must be at least `IFNAMSIZ` long. It can be an empty string, where the system will automatically generate the device name. Alternatively, a user may choose a specific `tunNN` device name. The return value is the descriptor to the opened `tun` device.
        
    *   `int ifreq_set_{addr|netmask|broadcast}(int fd, const char *dev, unsigned int addr)`: Set the IPv4 network, netmask, or broadcast address of a given network device `dev`. _**The `fd` parameter must be a valid socket, e.g., returned from socket(AF\_INET, SOCK\_DGRAM, IPPROTO\_UDP);**_
        
    *   `int ifreq_{set|get}_flag(int fd, const char *dev, ...)`: Set the flag value of a given network device `dev`. Generally, you must call `ifreq_get_flag` first to obtain the current flag value, modify the flag value, and then call `ifreq_set_flag` to update the flag value. To bring up a network interface, mark the `IFF_UP` in the flag using the `OR` operation.
        
    *   `MYADDR` and `NETMASK`: The server `tun` interface’s IP address and netmask. It is always `10.0.0.254/24`.
        
    *   `NIPQUAD`: A macro to print out a human-readable IP address using a format string. Suppose the IP address is stored in a variable `addr`. You can do something like `printf("%u.%u.%u.%u\n", NIPQUAD(addr));` to display an IP address.
        
    
    You may implement your server and client logic in `tunvpn_server()` and `tunvpn_client()` functions. If you like, you can also implement your own without following our sample codes.
    
4.  You can run the `make run` command to invoke the test (demo) environment. It creates three runtime instances in three panes of the `tmux` terminal multiplexer, as shown below.
    
    ![tmux3](https://inp.zoolab.org/netprog/lab08/tmux3.png)
    
    You can press `^B Up` or `^B Down` to switch between terminal panes. The top pane (server) is used for running the server program. The middle (client1) and the bottom (client2) panes are used for running the clients. Use the following two commands to run the server and the clients.
    
        /dist/run.sh server    # run the server
        /dist/run.sh client    # run a client
        
    
    You may also use the following commands to inspect the network settings of each terminal pane.
    
        ip link show
        ip addr show
        ping {server, client1, client2}
        iperf3 -c {server, client1, client2}
        # after the VPN network is up
        ping {10.0.0.254, 10.0.0.x, 10.0.0.y}
        iperf3 -c {10.0.0.254, 10.0.0.x, 10.0.0.y}
        
    
5.  The sample codes provided in the development packages only print out some messages. You must modify or rewrite it to complete the required features introduced in the [Demonstration](https://md.zoolab.org/s/svtj6_CJN#Demonstration) section.
    

[](https://md.zoolab.org/s/svtj6_CJN#Demonstration "Demonstration")Demonstration
--------------------------------------------------------------------------------

1.  **\[5 pts\]** You can run our development package, switch between `tmux` panes, and run the `run.sh` scripts in the `tmux` panes.
    
2.  **\[5 pts\]** You can (1) ping the server and client2 from client1 and (2) run `iperf3 -c` from client1 to perform throughput measurement against the server the client2. The commands invoked in the client1 are:
    
        ping server
        ping client2
        iperf3 -c server
        iperf3 -c client2
        
    
3.  **\[10 pts\]** Your implementation can configure `tun0` appropriately on the server. Your program has to configure `tun0` appropriately. The involved configurations include (1) setting the `MTU` to `1400`, (2) setting the address to `10.0.0.254/24`, and (3) bringing up the `tun0` interface. The output should look like
    
        e2d1e2524db6:/# ip link show tun0
        4: tun0: <POINTOPOINT,MULTICAST,NOARP,UP,LOWER_UP> mtu 1400 qdisc pfifo_fast state UNKNOWN mode DEFAULT group default qlen 500
            link/none
        e2d1e2524db6:/# ip addr show tun0
        4: tun0: <POINTOPOINT,MULTICAST,NOARP,UP,LOWER_UP> mtu 1400 qdisc pfifo_fast state UNKNOWN group default qlen 500
            link/none
            inet 10.0.0.254/24 brd 10.0.0.255 scope global tun0
               valid_lft forever preferred_lft forever
        
    
    The `tun0` interface should be removed automatically if the `tunvpn` server process is killed, e.g., using the `killall tunvpn` command. It should be implicitly done if the `tunvpn` server process creates the `tun` device.
    
4.  **\[10pts\]** The client1 can connect to the server, obtain an assigned network configuration from the server (`10.0.0.x/24`), create the `tun0` interface, and set up the `tun0` interface appropriately.
    
5.  **\[10pts\]** The client1 `10.0.0.x` can ping the VPN server `10.0.0.254` successfully, and vice versa.
    
6.  **\[20pts\]** The client1 `10.0.0.x` can successfully run `iperf3 -c` to perform throughput measurement against the VPN server address `10.0.0.254` (no crash nor hang), and vice versa.
    
7.  **\[10pts\]** The client2 can connect to the server, obtain an assigned network configuration from the server (`10.0.0.y/24`), create the `tun0` interface, and set up the `tun0` appropriately.
    
8.  **\[10pts\]** The client1 `10.0.0.x` can ping the client2 `10.0.0.y` successfully, and vice versa.
    
9.  **\[20pts\]** The client1 `10.0.0.x` can successfully run `iperf3 -c` to perform throughput measurement against the client2 `10.0.0.y` (no crash nor hang), and vice versa.
