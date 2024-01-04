#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PORT 12345

char buf[2097152];

int comp(const void* a, const void* b){
    return *(int *)a - *(int *)b;
}

long rrt(int sockfd, struct timeval start, struct timeval end){
    gettimeofday(&start, NULL);

    memset(buf, '\0', sizeof(buf));
	long bytesRead = recv(sockfd, buf, sizeof(buf), 0);
    buf[bytesRead] = '\0';

	gettimeofday(&end, NULL);
	long delay = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000;

    return delay;
}

int main(){
    // ------------ socket ----------
    int sockfd;
    struct sockaddr_in servaddr;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("client socket fail\n");
		return 0;
	}

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if((connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0){
		perror("client connect fail\n");
		return 0;
	}

	// ----------- delay --------------
	struct timeval start, end;
	long delay_ans = rrt(sockfd, start, end) / 2;
    long bw_ans = 0;
    
    // ------------ bw ----------------
    int totbw = 0, div = 0, packet = 0;
    int abw[100];
    memset(abw, 0, sizeof(abw));

    for(int i=0;i<100;i++){
        memset(buf, '\0', sizeof(buf));
        
        gettimeofday(&start, NULL);
        long bytesRead = recv(sockfd, buf, sizeof(buf), 0);
        gettimeofday(&end, NULL);

        double delay3 = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000.0;
        double tempbw = (bytesRead * 1000) / (128 * 1024 * delay3);

        // if(tempbw < 200) tempbw /= 0.935;
        // else if(tempbw < 275) tempbw /= 0.947;
        // else if(tempbw < 375) tempbw /= 0.953;
        // else if(tempbw < 475) tempbw /= 0.965;
        // else tempbw /= 0.965;
        // printf("%d : time = %lf | bw = %lf | recvbyte = %ld\n", i, delay3, tempbw, bytesRead);

        abw[packet] = tempbw;
        packet++;
    }

    qsort(abw, packet, sizeof(int), comp);
    for(int i = 15; i < 87; i++){
        // printf("%d : bw = %d\n", i, abw[i]);
        totbw += abw[i];
        div++;
    }
    bw_ans = totbw / div;

	printf("# RESULTS: delay = %ld ms, bandwidth = %ld Mbps\n", delay_ans, bw_ans);

    close(sockfd);
    return 0;
}
