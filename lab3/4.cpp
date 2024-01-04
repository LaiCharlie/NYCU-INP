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
string dd[4] = {"d","a","s","w"};
string rev_d[4] = {"a","d","w","s"};

char a[225][225];
char readmap[7][11];
char buf[32768];

struct timeval start, endtime;

void update(int x, int y){
    for(int i=-3;i<4;i++)
        for(int j=-5;j<6;j++)
            a[x+i][y+j] = readmap[3+i][5+j];
}

bool existmap = false;

void readmes(int sockfd, int x, int y){
    usleep(500000);
    memset(buf,'\0',sizeof(buf));
    int bytesRead = recv(sockfd, buf, sizeof(buf), 0);
    buf[bytesRead] = '\0';

    string temp = "";
    int linenum = 0;
    for (int i = 0; buf[i] != '\0'; i++){
        if(buf[i]!='\n')
            temp+=buf[i];
        else{
            if(temp.length()==18){
                for(int j=0;j<11;j++)
                    readmap[linenum][j] = temp.substr(7,11)[j];
                linenum++;
            }
            // if(temp.substr(0,6) == "BINGO!")
            //     cout << temp << '\n';
            cout << "Server response : " << temp << '\n';
            temp = "";
        }
    }
    if(linenum)
        existmap = true;
    cout << "\n-------\n\n";
}

void sendmes(int sockfd, string command){
    command += '\n';
    send(sockfd, command.c_str(), strlen(command.c_str()), 0);
}

bool visit[255][255];
bool finde = false;
bool timeout = false;
string path = "";

void dfs(int sockfd, int x, int y){
    if(finde)
        return;
    visit[x][y] = true;
    for(int i=0;i<4;i++){
        if(a[x+dx[i]][y+dy[i]]=='@'){
            cout << path << '\n';
            sendmes(sockfd, path);
            readmes(sockfd, x, y);
            update(x, y);
            path = "";
        }
        if(!visit[x+dx[i]][y+dy[i]] && a[x+dx[i]][y+dy[i]]!='#'){
            if(a[x+dx[i]][y+dy[i]]=='E'){
                finde = true;
                sendmes(sockfd, path + dd[i]);
                readmes(sockfd, x, y);
                return;
            }
            path += dd[i];
            dfs(sockfd, x+dx[i], y+dy[i]);
            if(finde || timeout) return;
            path += rev_d[i];
        }
    }
}

int main(){
    gettimeofday(&start, 0);
    // ------------ socket ----------
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("140.113.213.213");
    servaddr.sin_port = htons(10304);
    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    // ---------- buffer --------------
    // Maze #4: Explore and Escape from the Maze (Limited View!)
    memset(buf,'\0',sizeof(buf));
    int bytesRead = recv(sockfd, buf, sizeof(buf), 0);
    buf[bytesRead] = '\0';

    // ------------- dfs -------------------
    memset(visit, false, sizeof(visit));
    for(int i=0;i<225;i++)
        for(int j=0;j<225;j++)
            a[i][j] = '@';
    readmes(sockfd,112,112);
    if(existmap){
        update(112,112);
        existmap = false;
    }
    dfs(sockfd, 112, 112);

    close(sockfd);
    gettimeofday(&endtime, 0);
    int sec  = endtime.tv_sec  - start.tv_sec;
    int usec = endtime.tv_usec - start.tv_usec;
    cout << "\n\nFinal Exec Time = " << sec + (usec/1000000.0) << " sec\n\n";

    return 0;
}
