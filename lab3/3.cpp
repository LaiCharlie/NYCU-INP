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
#include <sys/time.h>
using namespace std;

int  dx[4] = {0, 0, 1, -1};
int  dy[4] = {1, -1, 0, 0};
char dd[4] = {'a','d','w','s'};
int n = 101;
int m = 101;
char a[125][125];

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
    servaddr.sin_port = htons(10303);
    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    // ---------- buffer --------------
    // Maze #3: Explore and Escape from the Maze
    char buf[32768];
    memset(buf,'\0',sizeof(buf));
    int bytesRead = recv(sockfd, buf, sizeof(buf), 0);
    buf[bytesRead] = '\0';

    string temp = "";
    for (int i = 0; buf[i] != '\0'; i++){
        if(buf[i]!='\n')
            temp+=buf[i];
        else
            temp="";
    }

    // -------- move the map to top of maze ----------
    memset(buf,'\0',sizeof(buf));
    bytesRead = recv(sockfd, buf, sizeof(buf), 0);
    buf[bytesRead] = '\0';
    int up = 102;
    for (int i = 0; buf[i] != '\0'; i++){
        if(buf[i]!='\n')
            temp+=buf[i];
        else{
            if(temp.length()==18)
                up = min(up,stoi(temp.substr(0,5)));
            // cout << temp << '\n';
            temp="";
        }
    }
    temp = "";
    for(int i=0;i<up;i++)
        temp += 'i';
    temp += '\n';
    send(sockfd, temp.c_str(), strlen(temp.c_str()), 0);
    temp = "";

    // -------- move the map to left of maze ----------
    bool mvleft = true;
    char buffer[4096];
    while(mvleft){
        memset(buffer,'\0',sizeof(buffer));
        bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
        buffer[bytesRead] = '\0';

        for (int i = 0; buffer[i] != '\0'; i++){
            if(buffer[i]!='\n')
                temp+=buffer[i];
            else{
                if(temp.length()==18){
                    if(temp[7]==' ')
                        mvleft = false;
                }
                // cout << temp << '\n';
                temp="";
            }
        }
        if(mvleft){
            string mov = "j\n";
            send(sockfd, mov.c_str(), strlen(mov.c_str()), 0);
        }
        else{
            string tempmov = "lll\n";
            send(sockfd, tempmov.c_str(), strlen(tempmov.c_str()), 0);
        }
    }

    // ----------- recv the mes of map ---------------

    for(int j=0;j<2;j++){
        memset(buffer,'\0',sizeof(buffer));
        bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
        buffer[bytesRead] = '\0';

        for (int i = 0; buffer[i] != '\0'; i++){
            if(buffer[i]!='\n')
                temp+=buffer[i];
            else{
                // cout << temp << ", len = "<< temp.length();
                // if(temp.length()==18)
                //     cout<<"   | map line = "<<stoi(temp.substr(0,5));
                // cout<<'\n';
                temp="";
            }
        }
        // cout << "\n ---- \n";
    }

    // ----------- fill the map matrix --------------
    memset(a,' ',sizeof(a));
    for(int x=0;x<15;x++){
        int y=0;
        int head = 0;
        while(true){
            memset(buffer,'\0',sizeof(buffer));
            bytesRead = recv(sockfd, buffer, sizeof(buffer), 0);
            buffer[bytesRead] = '\0';
            bool fm = false;
            for (int i = 0; buffer[i] != '\0'; i++){
                if(buffer[i]!='\n')
                    temp+=buffer[i];
                else{
                    // cout << temp << "   | len = "<< temp.length();
                    if(temp.length()==18){
                        y++;
                        fm = true;
                        // cout << "   | map line = " << stoi(temp.substr(0,5)) << "   |  y = " << y;
                        for(int j=0;j<11;j++)
                            a[stoi(temp.substr(0,5))][head+j] = temp[7+j];
                    }
                    // cout<<'\n';
                    temp="";
                }
            }
            // cout << "\n ---- \n";
            if(y==70) break;
            string tempmov = "lllllllllll\n";
            if(fm){
                send(sockfd, tempmov.c_str(), strlen(tempmov.c_str()), 0);
                head += 11;
            }
        }
        string rightmove(99,'j');
        string downmove(7,'k');
        string tempmov = rightmove + downmove + '\n';
        send(sockfd, tempmov.c_str(), strlen(tempmov.c_str()), 0);
    }

    // for(int i=0;i<101;i++){
    //     for(int j=0;j<101;j++)
    //         cout << a[i][j];
    //     cout << '\n';
    // }

    // ----------- send ans ----------------
    string ans = solve();
    ans += '\n';
    // cout << ans;
    send(sockfd, ans.c_str(), strlen(ans.c_str()), 0);
    
    char responseBuffer[8192];
    for(int i=0;i<3;i++){
        memset(responseBuffer,'\0',sizeof(responseBuffer));
        int responseBytesRead = recv(sockfd, responseBuffer, sizeof(responseBuffer), 0);
        responseBuffer[responseBytesRead] = '\0';
        // cout << '\n' << responseBuffer << '\n';
        // cout << "\n ---- \n";
    }

    cout << "\nReceived from server:";
    memset(responseBuffer,'\0',sizeof(responseBuffer));
    int responseBytesRead = recv(sockfd, responseBuffer, sizeof(responseBuffer), 0);
    responseBuffer[responseBytesRead] = '\0';
    cout << '\n' << responseBuffer;

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