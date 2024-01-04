
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PORT 12345
#define SIZ 2097152

int main(int argc, char* argv[]){
    struct sockaddr_in ser_addr;

    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_fd < 0){
        printf("Failed to create socket!\n");
        return -1;
    }

    int optval = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt failed\n");
        return 0;
    }

    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ser_addr.sin_port = htons(PORT);

    int ret = bind(server_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(ret < 0){
        printf("Socket bind failed!\n");
        return -1;
    }

    char buf[SIZ];
    socklen_t len;
    struct sockaddr_in client_addr;
    while(1){
        // memset(buf, '\0', SIZ);
        len = sizeof(client_addr);
        // int count = recvfrom(server_fd, buf, SIZ, 0, (struct sockaddr*)&client_addr, &len);
        // if(count == -1){
        //     printf("Receive data failed!\n");
        //     return -1;
        // }
        // printf("Client: %d\n", count);

        memset(buf, '\0', SIZ);
        buf[0] = 'a';
        sendto(server_fd, buf, 1, 0, (struct sockaddr*)&client_addr, len);
    }

    close(server_fd);
    return 0;
}