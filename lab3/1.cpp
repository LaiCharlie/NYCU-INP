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
int n = 7;
int m = 11;
char a[7][11];

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

    int arr[n][m];
    char pp[n][m];
    pair<int, int> las[n][m];
    memset(pp,'0',sizeof(pp));
    memset(arr, -1, sizeof(arr));
    queue <pair<int, int> >q;
    q.push(make_pair(sx, sy));
    arr[sx][sy] = 1;

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
    servaddr.sin_port = htons(10301);
    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    // ---------- buffer --------------

    char buffer[4096];
    memset(buffer,'\0',sizeof(buffer));
    int bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
    buffer[bytesRead] = '\0';
    // cout << buffer << endl;

    memset(buffer,'\0',sizeof(buffer));
    bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
    buffer[bytesRead] = '\0';
    // cout << buffer << endl;

    int row = 0, col = 0;
    for (int i = 0; buffer[i] != '\0'; i++) {
        if(buffer[i] == '#'){
            while(buffer[i]!='\n'){
                a[row][col] = buffer[i];
                col++;
                i++;
            }
            row++;
            col=0;
        }
    }

    // ---------- send ans ----------

    string ans = solve();
    ans += '\n';
    // cout << ans <<'\n';
    send(sockfd, ans.c_str(), strlen(ans.c_str()), 0);

    char responseBuffer[4096];
    memset(responseBuffer,'\0',sizeof(responseBuffer));
    int responseBytesRead = recv(sockfd, responseBuffer, sizeof(responseBuffer), 0);
    responseBuffer[responseBytesRead] = '\0';
    cout << "Received from server: " << responseBuffer << "\n\n";

    memset(responseBuffer,'\0',sizeof(responseBuffer));
    responseBytesRead = recv(sockfd, responseBuffer, sizeof(responseBuffer), 0);
    responseBuffer[responseBytesRead] = '\0';
    cout << responseBuffer << "\n";

    close(sockfd);
    gettimeofday(&end, 0);
    int sec  = end.tv_sec  - start.tv_sec;
    int usec = end.tv_usec - start.tv_usec;
    cout << "\n\nFinal Exec Time = " << sec + (usec/1000000.0) << " sec\n\n";
    
    return 0;
}