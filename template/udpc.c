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

#define SERVER_PORT 12345
#define BUFF_LEN 2097152
#define SERVER_IP "127.0.0.1"

int main(int argc, char* argv[]){
    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_fd < 0){
        printf("Failed to create socket!\n");
        return -1;
    }

    struct sockaddr_in ser_addr;
    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    ser_addr.sin_port = htons(SERVER_PORT);

    socklen_t len;
    struct sockaddr_in src;

    char buf[BUFF_LEN];
    len = sizeof(ser_addr);
    // sendto(client_fd, buf, BUFF_LEN, 0, (struct sockaddr*)&ser_addr, len);
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    memset(buf, 0, BUFF_LEN);
    recvfrom(client_fd, buf, BUFF_LEN, 0, (struct sockaddr*)&src, &len);
    gettimeofday(&end, NULL);

    double delay = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000.0;
    printf("# RESULTS: delay = %d ms, bandwidth = 0 Mbps\n", delay/2);
        
    sleep(1);

    close(client_fd);
    return 0;
}