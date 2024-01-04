#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
#include <string>
#include <cmath>
#include <ctime>
#include <stdio.h>
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
#include <sys/time.h>

using namespace std;

int main(int argc, char *argv[]){
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("140.113.213.213");
    servaddr.sin_port = htons(10314);

    string SERVER = "140.113.213.213";
    string PORT   = "10314";

    string token  = "";
    bool validotp = false;
    while(!validotp){
        // ---------- get otp --------------
        int sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
        connect(sockfd1, (struct sockaddr*)&servaddr, sizeof(servaddr));

        string stuid  = "110652034";
        string getotp = "GET /otp?name=" + stuid + " HTTP/1.1\r\nHost: " + SERVER + ":" + PORT + "\r\n\r\n";
        send(sockfd1, getotp.c_str(), strlen(getotp.c_str()), 0);
        usleep(300000);
        token = "";
        char buf[2048];
        memset(buf,'\0',sizeof(buf));
        while(recv(sockfd1, buf, sizeof(buf), 0)){
            for (int i = 0; buf[i] != '\0'; i++){
                if(i >= 155)
                    token += buf[i];
            }
        }
        // cout << token << '\n';
        close(sockfd1);

        // ------------- verify ------------
        usleep(500000);

        int sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
        connect(sockfd2, (struct sockaddr*)&servaddr, sizeof(servaddr));
        string verify = "GET /verify?otp=" + token + " HTTP/1.1\r\nHost: " + SERVER + ":" + PORT + "\r\n\r\n";
        send(sockfd2, verify.c_str(), strlen(verify.c_str()), 0);
        usleep(300000);
        string dryrun = "";
        memset(buf,'\0',sizeof(buf));
        while(recv(sockfd2, buf, sizeof(buf), 0)){
            for (int i = 0; buf[i] != '\0'; i++){
                if(i >= 154)
                    dryrun += buf[i];
            }
        }
        close(sockfd2);
        // cout << dryrun.substr(8) << '\n';
        if(dryrun.substr(8,2) == "OK")
            validotp = true;
        usleep(1000000);
    }

    string filepath = "out.txt";
    ofstream out;
    out.open(filepath);
    out << token;

    int sockfd3 = socket(AF_INET, SOCK_STREAM, 0);
    connect(sockfd3, (struct sockaddr*)&servaddr, sizeof(servaddr));
    string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
    string upload   = "POST /upload HTTP/1.1\r\n" + ("Host: " + SERVER + ":" + PORT + "\r\n") + ("Content-Type: multipart/form-data; boundary=" + boundary + "\r\n") + ("Content-Length: 294\r\n") + "\r\n" + ("--" + boundary + "\r\n") + ("Content-Disposition: form-data; name=\"file\"; filename=\"" + filepath + "\"\r\n") + ("Content-Type: text/plain\r\n\r\n") + token + "\r\n--" + boundary + "--\r\n";
    send(sockfd3, upload.c_str(), strlen(upload.c_str()), 0);
    
    usleep(300000);
    char buf[2048];
    memset(buf,'\0',sizeof(buf));
    while(recv(sockfd3, buf, sizeof(buf), 0)){
        for(int i=0;buf[i]!='\0';i++)
            if(i >= 154)
                cout << buf[i];
    }
    cout << '\n';
    close(sockfd3);

    return 0;
}
