/*
 *  Lab problem set for INP course
 *  by Chun-Ying Huang <chuang@cs.nctu.edu.tw>
 *  License: GPLv2
 */
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if.h>
#include <linux/if_tun.h>

using namespace std;

#define NIPQUAD(m)	((unsigned char*) &(m))[0], ((unsigned char*) &(m))[1], ((unsigned char*) &(m))[2], ((unsigned char*) &(m))[3]
#define errquit(m)	{ perror(m); exit(-1); }

#define MYADDR		0x0a0000fe // 10.0.0.254 // server
#define CLIADDR1    0x0a00000b // 10.0.0.11  // client1
#define CLIADDR2    0x0a00000c // 10.0.0.12  // client2
#define ADDRBASE	0x0a00000a // 10.0.0.10
#define	NETMASK		0xffffff00 // 255.255.255.0

// int tun_alloc(char *dev): allocate a tun device. The size of dev must be at least IFNAMSIZ long. 
// It can be an empty string, where the system will automatically generate the device name. 
// Alternatively, a user may choose a specific tunNN device name. 
// The return value is the descriptor to the opened tun device.
int tun_alloc(char *dev) {
	struct ifreq ifr;
	int fd, err;
	if((fd = open("/dev/net/tun", O_RDWR)) < 0 )
		return -1;
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;	/* IFF_TUN (L3), IFF_TAP (L2), IFF_NO_PI (w/ header) */
	if(dev && dev[0] != '\0') strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	if((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
		close(fd);
		return err;
	}
	if(dev) strcpy(dev, ifr.ifr_name);
	return fd;
}

int ifreq_set_mtu(int fd, const char *dev, int mtu) {
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_mtu = mtu;
	if(dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	return ioctl(fd, SIOCSIFMTU, &ifr);
}

int ifreq_get_flag(int fd, const char *dev, short *flag) {
	int err;
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	if(dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	err = ioctl(fd, SIOCGIFFLAGS, &ifr);
	if(err == 0) {
		*flag = ifr.ifr_flags;
	}
	return err;
}

int ifreq_set_flag(int fd, const char *dev, short flag) {
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	if(dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	ifr.ifr_flags = flag;
	return ioctl(fd, SIOCSIFFLAGS, &ifr);
}

int ifreq_set_sockaddr(int fd, const char *dev, int cmd, unsigned int addr) {
	struct ifreq ifr;
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = addr;
	memset(&ifr, 0, sizeof(ifr));
	memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
	if(dev) strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	return ioctl(fd, cmd, &ifr);
}

int ifreq_set_addr(int fd, const char *dev, unsigned int addr) {
	return ifreq_set_sockaddr(fd, dev, SIOCSIFADDR, addr);
}

int ifreq_set_netmask(int fd, const char *dev, unsigned int addr) {
	return ifreq_set_sockaddr(fd, dev, SIOCSIFNETMASK, addr);
}

int ifreq_set_broadcast(int fd, const char *dev, unsigned int addr) {
	return ifreq_set_sockaddr(fd, dev, SIOCSIFBRDADDR, addr);
}

char* get_server_ip(const char* serverName) {
    struct addrinfo hints, *result, *p;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(serverName, NULL, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return nullptr;
    }

    char* ipAddr = nullptr;
    for (p = result; p != nullptr; p = p->ai_next) {
        struct sockaddr_in* ipv4 = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);
        ipAddr = static_cast<char*>(malloc(INET_ADDRSTRLEN));
        if (ipAddr == nullptr) {
            perror("malloc");
            freeaddrinfo(result);
            return nullptr;
        }
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddr, INET_ADDRSTRLEN);
        break;
    }

    freeaddrinfo(result);
    return ipAddr;
}

void setupTUN(int &sock_fd, int &tun_fd, const char *ipaddr){
	char tun_name[5] = "tun0";
	short flg;
	sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    tun_fd  = tun_alloc(tun_name);
	ifreq_get_flag(sock_fd, tun_name, &flg);
	ifreq_set_flag(sock_fd, tun_name, (flg | IFF_UP));
    ifreq_set_mtu(sock_fd, tun_name, 1400);
    ifreq_set_addr(sock_fd, tun_name, inet_addr(ipaddr));
	ifreq_set_netmask(sock_fd, tun_name, htonl(NETMASK));
	ifreq_set_broadcast(sock_fd, tun_name, htonl(ADDRBASE | ~NETMASK));
}

int tunvpn_server(int port) {
	// tunvpn  ip : 10.0.0.254
	fprintf(stderr, "## [server] starts ...\n");

	// set up tunvpn interface
	int sock_fd, tun_fd;
	setupTUN(sock_fd, tun_fd, "10.0.0.254");

	int c = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct ifreq ifr{};
    strcpy(ifr.ifr_name, "eth0");
    ioctl(c, SIOCGIFADDR, &ifr);
    close(c);

    char ip[INET_ADDRSTRLEN];
    strcpy(ip, inet_ntoa(((sockaddr_in *) &ifr.ifr_addr)->sin_addr));
	printf("server ip address : %s\n", ip);

	// socket setting
	struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_port = htons(port);
	int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    bind(s, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

	struct sockaddr_in cli1;
    socklen_t cli_len1 = sizeof(cli1);
	struct sockaddr_in cli2;
    socklen_t cli_len2 = sizeof(cli2);

	struct timeval time_out;
	time_out.tv_sec  = 3;
	time_out.tv_usec = 0;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out, sizeof(struct timeval));

	unsigned int cli1_ip = 0;
	unsigned int cli2_ip = 0;

	while(1) { 
		char buffer[2097152];
		if(!cli1_ip){
			ssize_t recvLen1 = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*)&cli1, &cli_len1);

			cli1_ip = ntohl(cli1.sin_addr.s_addr);
			if(cli1_ip <= 2887523329 || cli1_ip >= 2887523333){
				cli1_ip = 0;
				continue;
			}
			if(recvLen1 > 0){
				char responseMessage[] = "10.0.0.11";
				sendto(s, responseMessage, strlen(responseMessage), 0, (struct sockaddr*)&cli1, sizeof(cli1));
				printf("first client\ntunvpn ip = %s\n", responseMessage);
				printf("client IP: %s\n", inet_ntoa(cli1.sin_addr));
			}
		}

		else if(!cli2_ip){
			ssize_t recvLen2 = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr*)&cli2, &cli_len2);
			if(cli1_ip == ntohl(cli2.sin_addr.s_addr)) continue;

			cli2_ip = ntohl(cli2.sin_addr.s_addr);
			if(cli2_ip <= 2887523329 || cli2_ip >= 2887523333){
				cli2_ip = 0;
				continue;
			}
			if(recvLen2 > 0){
				char responseMessage[] = "10.0.0.12";
				sendto(s, responseMessage, strlen(responseMessage), 0, (struct sockaddr*)&cli2, sizeof(cli2));
				printf("second client\ntunvpn ip = %s\n", responseMessage);
				printf("client IP: %s\n", inet_ntoa(cli2.sin_addr));
			}
		}

		else break;
	}

	while(1){
		fd_set read_fds;
		struct timeval time_out;
		FD_ZERO(&read_fds);
		FD_SET(s, &read_fds);
		FD_SET(tun_fd, &read_fds);

		time_out.tv_sec  = 5;
		time_out.tv_usec = 0;

		int max_fd = (s > tun_fd) ? s : tun_fd;
		int result = select(max_fd + 1, &read_fds, NULL, NULL, &time_out);
		if (result > 0) {
			if (FD_ISSET(s, &read_fds)) {
				char buffer[2097152];
				ssize_t bytes_read = read(s, buffer, sizeof(buffer));

				if (bytes_read > 0) {
					buffer[bytes_read] = '\0';
					// printf("server read %zd bytes from s: %s\n", bytes_read, buffer);
					struct iphdr *ip_header = (struct iphdr *)buffer;
					// printf("Version: %u\n", ip_header->version);
					// printf("Header length: %u\n", ip_header->ihl);
					// printf("Total length: %u\n", ntohs(ip_header->tot_len));
					// printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&(ip_header->saddr)));
					// printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&(ip_header->daddr)));
					if(strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->saddr)), "10.0.0.11") == 0 || strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->saddr)), "10.0.0.12") == 0){
						write(tun_fd, ip_header, bytes_read);
					}
				}
			}

			if (FD_ISSET(tun_fd, &read_fds)) {
				char buffer[2097152];
				ssize_t bytes_read = read(tun_fd, buffer, sizeof(buffer));

				if (bytes_read > 0) {
					buffer[bytes_read] = '\0';
					// printf("server read %zd bytes from tun_fd: %s\n", bytes_read, buffer);
					struct iphdr *ip_header = (struct iphdr *)buffer;
					// printf("Version: %u\n", ip_header->version);
					// printf("Header length: %u\n", ip_header->ihl);
					// printf("Total length: %u\n", ntohs(ip_header->tot_len));
					// printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&(ip_header->saddr)));
					// printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&(ip_header->daddr)));
					if(strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->daddr)), "10.0.0.11") == 0){
						sendto(s, ip_header, bytes_read, 0, (struct sockaddr*)&cli1, sizeof(cli1));
					}
					else if(strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->daddr)), "10.0.0.12") == 0){
						sendto(s, ip_header, bytes_read, 0, (struct sockaddr*)&cli2, sizeof(cli2));
					}
				}
			}
		}
	}

	close(s);
	close(sock_fd);
    close(tun_fd);

	return 0;
}

int tunvpn_client(const char *server, int port){
	fprintf(stderr, "## [client] starts ...\n");
	int c = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
	int tempfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct ifreq ifr{};
    strcpy(ifr.ifr_name, "eth0");
    ioctl(tempfd, SIOCGIFADDR, &ifr);
    close(tempfd);

    char own_ip[INET_ADDRSTRLEN];
    strcpy(own_ip, inet_ntoa(((sockaddr_in *) &ifr.ifr_addr)->sin_addr));

	printf("server ip = %s\n", get_server_ip(server));

	struct sockaddr_in serverAddr;
	socklen_t ser_len = sizeof(serverAddr);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(get_server_ip(server));
    serverAddr.sin_port = htons(port);

	struct timeval time_out;
	time_out.tv_sec  = 3;
	time_out.tv_usec = 0;
	setsockopt(c, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out, sizeof(struct timeval));
	bool setvpn = false;

	int sock_fd, tun_fd;
	string vpn_ip = "";

	while(1) {
		char mes[] = "client\n";
		sendto(c, mes, sizeof(mes), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

		char buffer[2097152];
		memset(buffer, '\0', sizeof('\0'));
        int recvLen = recvfrom(c, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddr, &ser_len);
		if(recvLen > 0){ 
			printf("Received packet from server at IP: %s\n", inet_ntoa(serverAddr.sin_addr));
			int i = 0;
			while (buffer[i] != '\0' && i < 9) {
				vpn_ip += buffer[i];
				i++;
			}

			if(!setvpn){
				setvpn = true;
				cout << "tun vpn ip = " << vpn_ip << '\n';
				setupTUN(sock_fd, tun_fd, vpn_ip.c_str());
			}
			break;
		}
	}

	while(1){
		fd_set read_fds;
		struct timeval time_out;
		FD_ZERO(&read_fds);
		FD_SET(c, &read_fds);
		FD_SET(tun_fd, &read_fds);

		time_out.tv_sec  = 5;
		time_out.tv_usec = 0;

		int max_fd = (c > tun_fd) ? c : tun_fd;
		int result = select(max_fd + 1, &read_fds, NULL, NULL, &time_out);
		if (result > 0) {
			if (FD_ISSET(c, &read_fds)) {
				char buffer[2097152];
				ssize_t bytes_read = read(c, buffer, sizeof(buffer));

				if (bytes_read > 0) {
					buffer[bytes_read] = '\0';
					struct iphdr *ip_header = (struct iphdr *)buffer;
					// printf("client read %zd bytes from c: %s\n", bytes_read, buffer);
					// printf("Version: %u\n", ip_header->version);
					// printf("Header length: %u\n", ip_header->ihl);
					// printf("Total length: %u\n", ntohs(ip_header->tot_len));
					// printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&(ip_header->saddr)));
					// printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&(ip_header->daddr)));
					if((strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->saddr)), "10.0.0.254") == 0 || strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->saddr)), "10.0.0.11") == 0 || strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->saddr)), "10.0.0.12") == 0) && strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->saddr)), vpn_ip.c_str()) != 0){
						write(tun_fd, ip_header, bytes_read);
					}
				}
			}

			if (FD_ISSET(tun_fd, &read_fds)) {
				char buffer[2097152];
				ssize_t bytes_read = read(tun_fd, buffer, sizeof(buffer));

				if (bytes_read > 0) {
					buffer[bytes_read] = '\0';
					struct iphdr *ip_header = (struct iphdr *)buffer;
					// printf("client read %zd bytes from tun_fd: %s\n", bytes_read, buffer);
					// printf("Version: %u\n", ip_header->version);
					// printf("Header length: %u\n", ip_header->ihl);
					// printf("Total length: %u\n", ntohs(ip_header->tot_len));
					// printf("Source IP: %s\n", inet_ntoa(*(struct in_addr *)&(ip_header->saddr)));
					// printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&(ip_header->daddr)));
					if((strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->daddr)), "10.0.0.254") == 0 || strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->daddr)), "10.0.0.11") == 0 || strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->daddr)), "10.0.0.12") == 0) && strcmp(inet_ntoa(*(struct in_addr *)&(ip_header->daddr)), vpn_ip.c_str()) != 0){
						sendto(c, ip_header, bytes_read, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
					}
				}
			}
		}
	}
	
	close(tun_fd);
	close(sock_fd);
    close(c);
	return 0;
}

int usage(const char *progname) {
	fprintf(stderr, "usage: %s {server|client} {options ...}\n"
		"# server mode:\n"
		"	%s server port\n"
		"# client mode:\n"
		"	%s client servername serverport\n",
		progname, progname, progname);
	return -1;
}

int main(int argc, char *argv[]) {
	if(argc < 3) {
		return usage(argv[0]);
	}
	if(strcmp(argv[1], "server") == 0) {
		if(argc < 3) return usage(argv[0]);
		return tunvpn_server(strtol(argv[2], NULL, 0));
	} 
	else if(strcmp(argv[1], "client") == 0) {
		if(argc < 4) return usage(argv[0]);
		return tunvpn_client(argv[2], strtol(argv[3], NULL, 0));
	} 
	else {
		fprintf(stderr , "## unknown mode %s\n", argv[1]);
	}
	return 0;
}

// /dist/run.sh server
// /dist/run.sh client

// docker network prune

// ip link show
// ip addr show eth0
// ping {server, client1, client2}
// iperf3 -c {server, client1, client2}
// # after the VPN network is up
// ping {10.0.0.254, 10.0.0.11, 10.0.0.12}
// iperf3 -c {10.0.0.254, 10.0.0.11, 10.0.0.12}