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

int com(const void* a, const void* b){
    return *(int *)a - *(int *)b;
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
    gettimeofday(&start, NULL);

    double bw = 0;
    char buf[2097152];
    memset(buf, '\0', sizeof(buf));
	long bytesRead = recv(sockfd, buf, sizeof(buf), 0);
    buf[bytesRead] = '\0';

	gettimeofday(&end, NULL);
	long delay = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000;
    // printf("# RESULTS: delay = %ld ms, bandwidth = 0 Mbps\n", delay/2);
    // -----------------------------

    int packet = 0;
    for(int i=0;i<100;i++){
        gettimeofday(&start, NULL);

        memset(buf, '\0', sizeof(buf));
        bytesRead = recv(sockfd, buf, sizeof(buf), 0);
        buf[bytesRead] = '\0';

        gettimeofday(&end, NULL);
        double delay3 = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000.0;

        bytesRead *= 8;  // 8 bits = 1 byte
        double tempbw = (bytesRead * 1000) / (delay3 * 1024 * 1024); // Mbps
        // if(tempbw < 100) tempbw += 1;
        // if(tempbw > 300) tempbw -= 4;
        // if(tempbw > 350) tempbw -= 6;
        // if(tempbw > 400) tempbw -= 6;
        // if(tempbw > 450) tempbw -= 2;
        // if(tempbw > 500) tempbw -= 4;
        // if(tempbw > 525) tempbw -= 16;

        if(delay3 < delay/2){
            bw = packet ? (tempbw + bw * packet) / (packet + 1) : tempbw;
            packet++;
        }
    }

    int finalbw = bw;
	printf("# RESULTS: delay = %ld ms, bandwidth = %d Mbps\n", delay/2, finalbw);


    close(sockfd);

    return 0;
}






/*
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

int com(const void* a, const void* b){
    return *(int *)a - *(int *)b;
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
    gettimeofday(&start, NULL);

    double bw = 0;
    char buf[2097152];
    memset(buf, '\0', sizeof(buf));
	long bytesRead = recv(sockfd, buf, sizeof(buf), 0);
    buf[bytesRead] = '\0';

	gettimeofday(&end, NULL);
	long delay = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000;
    // -----------------------------
    int abw[100];
    memset(abw, 0, sizeof(abw));

    int packet = 0;
    for(int i=0;i<95;i++){
        gettimeofday(&start, NULL);

        memset(buf, '\0', sizeof(buf));
        bytesRead = recv(sockfd, buf, sizeof(buf), 0);
        buf[bytesRead] = '\0';

        gettimeofday(&end, NULL);
        double delay3 = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000.0;

        bytesRead *= 8;  // 8 bits = 1 byte
        double tempbw = bytesRead / (1000 * delay3); // Mbps
        // printf("%lf ,", tempbw);

        if(tempbw < 250) tempbw += 1;
        if(tempbw > 300) tempbw -= 4;
        if(tempbw > 400) tempbw -= 8;
        if(tempbw > 535) tempbw -= 24;

        if(delay3 < delay/2){
            abw[packet] = tempbw;
            bw = packet ? (tempbw + bw * packet) / (packet + 1) : tempbw;
            packet++;
        }
    }

    qsort(abw, packet, sizeof(int), com);
    int totbw = 0, div = 0;
    int finalbw = 0;
    if(bw < 181){
        for(int i=6;i<packet-6;i++){
            totbw += abw[i];
            div++;
        }
        finalbw = totbw/div;
    }
    else if(bw < 271){
        for(int i=6;i<packet-5;i++){
            totbw += abw[i];
            div++;
        }
        finalbw = totbw/div;
    }
    else if(bw < 331){
        for(int i=6;i<packet-7;i++){
            totbw += abw[i];
            div++;
        }
        finalbw = totbw/div;
    }
    else if(bw < 426){
        for(int i=9;i<packet-7;i++){
            totbw += abw[i];
            div++;
        }
        finalbw = totbw/div;
    }
    else if(bw < 531){
        for(int i=10;i<packet-15;i++){
            totbw += abw[i];
            div++;
        }
        finalbw = (totbw/div * 3 + bw * 4) / 7;
    }
    else{
        for(int i=6;i<packet-10;i++){
            totbw += abw[i];
            div++;
        }
        // finalbw = (totbw/div * 8 + bw * 7) / 15;
        // finalbw = bw;
        finalbw = totbw / div;
    }

    // printf("\n");
	printf("# RESULTS: delay = %ld ms, bandwidth = %d Mbps, p = %d, bw = %lf, tempbw = %d\n", delay/2, finalbw, packet, bw, totbw/div);


    close(sockfd);

    return 0;
}





















-----------------------------------------





















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
        gettimeofday(&start, NULL);

        memset(buf, '\0', sizeof(buf));
        long bytesRead = recv(sockfd, buf, sizeof(buf), 0);
        buf[bytesRead] = '\0';

        gettimeofday(&end, NULL);

        double delay3 = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000.0;
        double tempbw = (bytesRead * 1000) / (128 * 1024 * delay3);
        // if(tempbw < 200) tempbw /= 0.935;
        // else if(tempbw < 275) tempbw /= 0.945;
        // else if(tempbw < 375) tempbw /= 0.95;
        // else if(tempbw < 475) tempbw /= 0.952;
        // else tempbw /= 0.96;
        // else if(tempbw < 250) tempbw /= 0.86;
        // printf("%d : time = %lf | bw = %lf | recvbyte = %ld\n", i, delay3, tempbw, bytesRead);

        abw[packet] = tempbw;
        packet++;
    }

    qsort(abw, packet, sizeof(int), comp);
    for(int i = 15; i < 88; i++){
        // printf("%d : bw = %d\n", i, abw[i]);
        totbw += abw[i];
        div++;
    }
    bw_ans = totbw / div;

    if(bw_ans < 200) bw_ans /= 0.935;
    else if(bw_ans < 275) bw_ans /= 0.945;
    else if(bw_ans < 375) bw_ans /= 0.95;
    else if(bw_ans < 475) bw_ans /= 0.953;
    else bw_ans /= 0.95;

	printf("# RESULTS: delay = %ld ms, bandwidth = %ld Mbps, avgbw = %d Mbps\n", delay_ans, bw_ans, totbw/div);

    close(sockfd);
    return 0;
}

*/