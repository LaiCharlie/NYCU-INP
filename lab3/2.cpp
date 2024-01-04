#include <iostream>
#include <queue>
#include <algorithm>
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

int  dx[4] = {0, 0, 1, -1};
int  dy[4] = {1, -1, 0, 0};
char dd[4] = {'a','d','w','s'};
int n = 21;
int m = 79;
char a[21][79];

string solve(){
    int num, sx, sy, tx, ty;
    for (int j = 0; j < n; j++){
        for (int k = 0; k < m; k++){
            if(a[j][k]=='*'){
                sx = j;
                sy = k;
            }
            if(a[j][k]=='E'){
                tx = j;
                ty = k;
            }
        }
    }

    a[tx][ty] = '.';
    
    pair<int, int> las[n][m];
    int arr[n][m];
    memset(arr, -1, sizeof(arr));
    arr[sx][sy] = 1;

    queue <pair<int, int> >q;
    q.push(make_pair(sx, sy));
    while (!q.empty()){
        pair<int, int> now;
        now = q.front();
        q.pop();
        for (int i = 0; i < 4; i++){
            if (now.first+dx[i] >= 0 && now.first+dx[i] < n && now.second+dy[i] >= 0 && now.second+dy[i] < m){
                if (a[now.first+dx[i]][now.second+dy[i]] == '.' && arr[now.first+dx[i]][now.second+dy[i]] == -1){
                    q.push(make_pair(now.first+dx[i], now.second+dy[i]));
                    arr[now.first+dx[i]][now.second+dy[i]] = arr[now.first][now.second] + 1;
                    las[now.first+dx[i]][now.second+dy[i]] = make_pair(now.first, now.second);
                }
            }
        }
    }

    int nnx=tx,nny=ty;

    string pth = "";

    while(true){
        if(nnx == sx && nny == sy)
            break;
        int tmpx  = las[nnx][nny].first;
        int tmpy  = las[nnx][nny].second;
        for (int i = 0; i < 4; i++){
            if ((tmpx-nnx)==dx[i] && (tmpy-nny)==dy[i])
                pth += dd[i];
        }
        nnx = tmpx;
        nny = tmpy;
    }
    reverse(pth.begin(),pth.end());
    return pth;
}

int main(){
    struct timeval start, end;
    gettimeofday(&start, 0);
    // ------------ socket ----------
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("140.113.213.213");
    servaddr.sin_port = htons(10302);
    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    // ---------- buffer --------------

    char buf[1024];
    memset(buf,'\0',sizeof(buf));
    int bytesRead = recv(sockfd, buf, sizeof(buf), 0);
    buf[bytesRead] = '\0';
    // cout << buf << endl;

    string temp = "";

    int num = 0;
    for (int i = 0; buf[i] != '\0'; i++){
        if(buf[i]!='\n'){
            temp+=buf[i];
        }
        else{
            num++;
            // cout<<"Str "<<num<<" : "<<temp<<'\n';
            if(num > 9 && num < 30){
                for(int j=0;j<79;j++)
                    a[num-9][j] = temp[j];
            }
            temp="";
        }
    }

    char buffer[32768];
    memset(buffer,'\0',sizeof(buffer));
    bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
    buffer[bytesRead] = '\0';
    // cout << buffer << endl;

    for (int i = 0; buffer[i] != '\0'; i++){
        if(buffer[i]!='\n'){
            temp+=buffer[i];
        }
        else{
            num++;
            // cout<<"Str "<<num<<" : "<<temp<<'\n';
            if(num > 9 && num < 30){
                for(int j=0;j<79;j++)
                    a[num-9][j] = temp[j];
            }
            temp="";
        }
    }

    memset(buffer,'\0',sizeof(buffer));
    bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
    buffer[bytesRead] = '\0';
    // cout << buffer << endl;

    for (int i = 0; buffer[i] != '\0'; i++){
        if(buffer[i]!='\n'){
            temp+=buffer[i];
        }
        else{
            num++;
            // cout<<"Str "<<num<<" : "<<temp<<'\n';
            if(num > 9 && num < 30){
                for(int j=0;j<79;j++)
                    a[num-9][j] = temp[j];
            }
            temp="";
        }
    }

    string ans = solve();
    ans += '\n';
    // cout << ans;
    send(sockfd, ans.c_str(), strlen(ans.c_str()), 0);

    char responseBuffer[8192];
    memset(responseBuffer,'\0',sizeof(responseBuffer));
    int responseBytesRead = recv(sockfd, responseBuffer, sizeof(responseBuffer), 0);
    responseBuffer[responseBytesRead] = '\0';
    cout << "Received from server:  " << responseBuffer << "\n\n";

    memset(responseBuffer,'\0',sizeof(responseBuffer));
    responseBytesRead = recv(sockfd, responseBuffer, sizeof(responseBuffer), 0);
    responseBuffer[responseBytesRead] = '\0';
    cout << responseBuffer << '\n';

    close(sockfd);
    gettimeofday(&end, 0);
    int sec  = end.tv_sec  - start.tv_sec;
    int usec = end.tv_usec - start.tv_usec;
    cout << "\n\nFinal Exec Time = " << sec + (usec/1000000.0) << " sec\n\n";

    return 0;
}