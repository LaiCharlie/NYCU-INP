#include <iostream>
#include <queue>
#include <algorithm>
#include <string>
#include <cmath>
#include <ctime>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

int main(){
    // ------------ socket ----------
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);   // TCP
    // sockfd = socket(AF_INET, SOCK_DGRAM, 0);    // UDP
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("140.113.213.213");
    servaddr.sin_port = htons(10301);
    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    // ---------- buffer --------------
    char buf[32768];
    memset(buf,'\0',sizeof(buf));
    int bytesRead = recv(sockfd, buf, sizeof(buf), 0);
    buf[bytesRead] = '\0';

    // -------- separate the response of server by '\n' --------
    string temp = "";
    for (int i = 0; buf[i] != '\0'; i++){
        if(buf[i]!='\n')
            temp+=buf[i];
        else{
            cout << temp << '\n';
            temp="";
        }
    }

    // ----------- command send ---------
    string ans = "command\n";
    send(sockfd, ans.c_str(), strlen(ans.c_str()), 0);
    
    // ---------- buffer --------------
    char responseBuffer[8192];
    cout << "\nReceived from server:";
    memset(responseBuffer,'\0',sizeof(responseBuffer));
    int responseBytesRead = recv(sockfd, responseBuffer, sizeof(responseBuffer), 0);
    responseBuffer[responseBytesRead] = '\0';

    // ------- cout the whole response ----------
    cout << '\n' << responseBuffer << '\n';

    close(sockfd);

    return 0;
}