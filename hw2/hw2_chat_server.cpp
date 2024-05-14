#include <map>
#include <string>
#include <vector>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024
#define MAX_EVENTS  1500

using namespace std;

typedef struct user_t {
    string username;
    int    login;
    int    chat;
    struct sockaddr_in cliaddr;
} user_t;

typedef struct account {
    char username[50];
    char password[50];
    char status[50];
    int  login;
    int  chat;
} account;

typedef struct room {
    int number;
    string owner;
    vector<string> mes;
} room;

map<int, room>   chatroom;
map<int, user_t> connect_table;
map<string, account> name_list;
map<string, string> filter;

void init(){
    filter["=="] = "**";
    filter["hello"] = "*****";
    filter["superpie"] = "********";
    filter["starburst stream"] = "****************";
    filter["domain expansion"] = "****************";
}

int broadcast(char *msg, int len, int room_num) {
    map<int, user_t>::iterator it;
    for (it = connect_table.begin(); it != connect_table.end(); it++) {
        if(it->second.chat == room_num)
            send(it->first, msg, len, 0);
    }
    return 0;
}

bool valid(char c){
    return (c != '\n') && (c != ' ') && (c != '\0');
}

int isInteger(const char *str) {
    char *endptr;
    strtol(str, &endptr, 10);
    return (*str != '\0' && *endptr == '\0');
}

int handle_read(int to_handle) {
    char buff[BUFFER_SIZE];
    memset(buff, '\0', sizeof(buff));

    int nread = read(to_handle, buff, sizeof(buff));
    if (nread == 0) return -1;
    buff[nread] = '\0';

    vector<string> command;
    string temp = "";
    for(int i = 0; i < nread + 1; i++){
        if(valid(buff[i])) 
            temp += buff[i];
        else if(temp.length()){
            command.push_back(temp);
            temp = "";
        }
    }

    char msg[2048];
    if(connect_table[to_handle].chat == 0){
        if(command.size() < 1){
            sprintf(msg, "Error: Unknown command\n");
            send(to_handle, msg, strlen(msg), 0);
            char tmp[] = "% ";
            send(to_handle, tmp, strlen(tmp), 0);
        }

        else if(strcasecmp(command[0].c_str(), "register") == 0){
            if(command.size() != 3){
                sprintf(msg, "Usage: register <username> <password>\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else if(name_list.count(command[1])){
                sprintf(msg, "Username is already used.\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else{
                sprintf(msg, "Register successfully.\n");
                send(to_handle, msg, strlen(msg), 0);

                account account_info;
                strcpy(account_info.username, command[1].c_str());
                strcpy(account_info.password, command[2].c_str());
                sprintf(account_info.status, "offline");
                account_info.login = 0;
                account_info.chat  = 0;
                name_list[command[1]] = account_info;
            }
            char tmp[] = "% ";
            send(to_handle, tmp, strlen(tmp), 0);
        }

        else if(strcasecmp(command[0].c_str(), "login") == 0){
            if(command.size() != 3){
                sprintf(msg, "Usage: login <username> <password>\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else if(connect_table[to_handle].login == 1){
                sprintf(msg, "Please logout first.\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else if(!name_list.count(command[1]) || strcmp(name_list[command[1]].password, command[2].c_str()) || name_list[command[1]].login){
                sprintf(msg, "Login failed.\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else{
                sprintf(msg, "Welcome, %s.\n", command[1].c_str());
                send(to_handle, msg, strlen(msg), 0);

                connect_table[to_handle].login    = 1;
                connect_table[to_handle].username = command[1];

                name_list[command[1]].login  = 1;
                sprintf(name_list[command[1]].status, "online");
            }
            char tmp[] = "% ";
            send(to_handle, tmp, strlen(tmp), 0);
        }

        else if(strcasecmp(command[0].c_str(), "logout") == 0){
            if(command.size() != 1){
                sprintf(msg, "Usage: logout\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else if(connect_table[to_handle].login == 0){
                sprintf(msg, "Please login first.\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else{
                sprintf(msg, "Bye, %s.\n", connect_table[to_handle].username.c_str());
                send(to_handle, msg, strlen(msg), 0);

                connect_table[to_handle].login = 0;
                name_list[connect_table[to_handle].username].login = 0;
                name_list[connect_table[to_handle].username].chat  = 0;
                sprintf(name_list[connect_table[to_handle].username].status, "offline");
            }
            char tmp[] = "% ";
            send(to_handle, tmp, strlen(tmp), 0);
        }

        else if(strcasecmp(command[0].c_str(), "exit") == 0){
            if(command.size() != 1){
                sprintf(msg, "Usage: exit\n");
                send(to_handle, msg, strlen(msg), 0);
                char tmp[] = "% ";
                send(to_handle, tmp, strlen(tmp), 0);
            }
            else if(connect_table[to_handle].login){
                sprintf(msg, "Bye, %s.\n", connect_table[to_handle].username.c_str());
                send(to_handle, msg, strlen(msg), 0);
                return -1;
            }
            else
                return -1;
        }

        else if(strcasecmp(command[0].c_str(), "whoami") == 0){
            if(command.size() != 1){
                sprintf(msg, "Usage: whoami\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else if(connect_table[to_handle].login == 0){
                sprintf(msg, "Please login first.\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else{
                sprintf(msg, "%s\n", connect_table[to_handle].username.c_str());
                send(to_handle, msg, strlen(msg), 0);
            }
            char tmp[] = "% ";
            send(to_handle, tmp, strlen(tmp), 0);
        }

        else if(strcasecmp(command[0].c_str(), "set-status") == 0){
            if(command.size() != 2){
                sprintf(msg, "Usage: set-status <status>\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else if(connect_table[to_handle].login == 0){
                sprintf(msg, "Please login first.\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else{
                if(strcasecmp(command[1].c_str(), "online") == 0){
                    sprintf(name_list[connect_table[to_handle].username].status, "online");
                    sprintf(msg, "%s %s\n", connect_table[to_handle].username.c_str(), name_list[connect_table[to_handle].username].status);
                    send(to_handle, msg, strlen(msg), 0);
                }
                else if(strcasecmp(command[1].c_str(), "offline") == 0){
                    sprintf(name_list[connect_table[to_handle].username].status, "offline");
                    sprintf(msg, "%s %s\n", connect_table[to_handle].username.c_str(), name_list[connect_table[to_handle].username].status);
                    send(to_handle, msg, strlen(msg), 0);
                }
                else if(strcasecmp(command[1].c_str(), "busy") == 0){
                    sprintf(name_list[connect_table[to_handle].username].status, "busy");
                    sprintf(msg, "%s %s\n", connect_table[to_handle].username.c_str(), name_list[connect_table[to_handle].username].status);
                    send(to_handle, msg, strlen(msg), 0);
                }
                else{
                    sprintf(msg, "set-status failed\n");
                    send(to_handle, msg, strlen(msg), 0);
                }
            }
            char tmp[] = "% ";
            send(to_handle, tmp, strlen(tmp), 0);
        }

        else if(strcasecmp(command[0].c_str(), "list-user") == 0){
            if(command.size() != 1){
                sprintf(msg, "Usage: list-user\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else if(connect_table[to_handle].login == 0){
                sprintf(msg, "Please login first.\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else{
                map<string, account>::iterator it;
                for (it = name_list.begin(); it != name_list.end(); it++) {
                    sprintf(msg, "%s %s\n", it->second.username, it->second.status);
                    send(to_handle, msg, strlen(msg), 0);
                }
            }
            char tmp[] = "% ";
            send(to_handle, tmp, strlen(tmp), 0);
        }

        // --------- chat room ------------

        else if(strcasecmp(command[0].c_str(), "enter-chat-room") == 0){
            if(command.size() != 2){
                sprintf(msg, "Usage: enter-chat-room <number>\n");
                send(to_handle, msg, strlen(msg), 0);
                char tmp[] = "% ";
                send(to_handle, tmp, strlen(tmp), 0);
            }
            else if(connect_table[to_handle].login == 0){
                sprintf(msg, "Please login first.\n");
                send(to_handle, msg, strlen(msg), 0);
                char tmp[] = "% ";
                send(to_handle, tmp, strlen(tmp), 0);
            }
            else if(!isInteger(command[1].c_str()) || (atoi(command[1].c_str()) < 1) || (atoi(command[1].c_str()) > 100)){
                sprintf(msg, "Number %s is not valid.\n", command[1].c_str());
                send(to_handle, msg, strlen(msg), 0);
                char tmp[] = "% ";
                send(to_handle, tmp, strlen(tmp), 0);
            }
            else if(!chatroom.count(atoi(command[1].c_str()))){
                connect_table[to_handle].chat = atoi(command[1].c_str());
                name_list[connect_table[to_handle].username].chat = atoi(command[1].c_str());

                chatroom[atoi(command[1].c_str())].number = atoi(command[1].c_str());
                chatroom[atoi(command[1].c_str())].owner  = connect_table[to_handle].username;
                string fl = "Welcome to the public chat room.\n";
                string sl = "Room number: " + to_string(chatroom[atoi(command[1].c_str())].number) + "\n";
                string tl = "Owner: " + connect_table[to_handle].username + "\n";
                send(to_handle, fl.c_str(), strlen(fl.c_str()), 0);
                send(to_handle, sl.c_str(), strlen(sl.c_str()), 0);
                send(to_handle, tl.c_str(), strlen(tl.c_str()), 0);
            }
            else{
                connect_table[to_handle].chat = atoi(command[1].c_str());
                name_list[connect_table[to_handle].username].chat = atoi(command[1].c_str());

                string fl = "Welcome to the public chat room.\n";
                string sl = "Room number: " + to_string(chatroom[atoi(command[1].c_str())].number) + "\n";
                string tl = "Owner: " + chatroom[atoi(command[1].c_str())].owner + "\n";
                send(to_handle, fl.c_str(), strlen(fl.c_str()), 0);
                send(to_handle, sl.c_str(), strlen(sl.c_str()), 0);
                send(to_handle, tl.c_str(), strlen(tl.c_str()), 0);

                int sz = chatroom[atoi(command[1].c_str())].mes.size();
                int his_num = min(20, sz);
                for(int i = his_num; i > 0; i--){
                    send(to_handle, chatroom[atoi(command[1].c_str())].mes[sz - i].c_str(), strlen(chatroom[atoi(command[1].c_str())].mes[sz - i].c_str()), 0);
                }
                
                sprintf(msg, "%s had enter the chat room.\n", connect_table[to_handle].username.c_str());
                map<int, user_t>::iterator it;
                for (it = connect_table.begin(); it != connect_table.end(); it++) {
                    if(it->second.chat == chatroom[atoi(command[1].c_str())].number && it->first != to_handle)
                        send(it->first, msg, strlen(msg), 0);
                }
            }
        }

        else if(strcasecmp(command[0].c_str(), "list-chat-room") == 0){
            if(command.size() != 1){
                sprintf(msg, "Usage: list-chat-room\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else if(connect_table[to_handle].login == 0){
                sprintf(msg, "Please login first.\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else{
                map<int, room>::iterator it;
                for (it = chatroom.begin(); it != chatroom.end(); it++) {
                    sprintf(msg, "%s %d\n", it->second.owner.c_str(), it->first);
                    send(to_handle, msg, strlen(msg), 0);
                }
            }
            char tmp[] = "% ";
            send(to_handle, tmp, strlen(tmp), 0);
        }

        else if(strcasecmp(command[0].c_str(), "close-chat-room") == 0){
            if(command.size() != 2){
                sprintf(msg, "Usage: close-chat-room <number>\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else if(connect_table[to_handle].login == 0){
                sprintf(msg, "Please login first.\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else if(!chatroom.count(atoi(command[1].c_str()))){
                sprintf(msg, "Chat room %d does not exist.\n", atoi(command[1].c_str()));
                send(to_handle, msg, strlen(msg), 0);
            }
            else if(strcmp(chatroom[atoi(command[1].c_str())].owner.c_str(), connect_table[to_handle].username.c_str())){
                sprintf(msg, "Only the owner can close this chat room.\n");
                send(to_handle, msg, strlen(msg), 0);
            }
            else{
                sprintf(msg, "Chat room %d was closed.\n", atoi(command[1].c_str()));
                map<string, account>::iterator itt;
                for (itt = name_list.begin(); itt != name_list.end(); itt++) {
                    if(name_list[itt->first].chat == atoi(command[1].c_str()))
                        name_list[itt->first].chat = 0;
                }

                send(to_handle, msg, strlen(msg), 0);
                map<int, user_t>::iterator it;
                for (it = connect_table.begin(); it != connect_table.end(); it++) {
                    if(connect_table[it->first].chat == atoi(command[1].c_str())){
                        char tmp[] = "% ";
                        send(it->first, msg, strlen(msg), 0);
                        send(it->first, tmp, strlen(tmp), 0);
                        connect_table[it->first].chat = 0;
                    }
                }

                chatroom.erase(atoi(command[1].c_str()));
            }
            char tmp[] = "% ";
            send(to_handle, tmp, strlen(tmp), 0);
        }

        else{
            sprintf(msg, "Error: Unknown command\n");
            send(to_handle, msg, strlen(msg), 0);
            char tmp[] = "% ";
            send(to_handle, tmp, strlen(tmp), 0);
        }
    }


    else{
        string buf = "";
        for(int i = 0; i < nread; i++) buf += tolower(buff[i]);
        for (auto it = filter.begin(); it != filter.end(); ++it) {
            size_t pos = 0;
            while ((pos = buf.find(it->first, pos)) != string::npos) {
                buf.replace(pos, it->first.length(), it->second);
                pos += it->second.length();
            }
        }

        for(int i = 0; i < nread; i++)
            if(buf[i] == '*') 
                buff[i] = '*';

        // sprintf(msg, "buf : ");
        // send(to_handle, msg, strlen(msg), 0);
        // send(to_handle, buf.c_str(), strlen(buf.c_str()), 0);

        if(command.size() > 0 && strcasecmp(command[0].c_str(), "/pin") == 0){
            string pin_mes = "";
            for(int i = 5; i < int(strlen(buff)); i++) pin_mes += buff[i];
            if(pin_mes.length() == 0) pin_mes = "\n";

            for (vector<string>::iterator it = chatroom[connect_table[to_handle].chat].mes.begin(); it != chatroom[connect_table[to_handle].chat].mes.end();) {
                if (it->compare(0, 3, "Pin") == 0) 
                    it = chatroom[connect_table[to_handle].chat].mes.erase(it, it + 2);
                else 
                    it++;
            }

            sprintf(msg, "Pin -> [%s]: ", connect_table[to_handle].username.c_str());
            broadcast(msg, strlen(msg), connect_table[to_handle].chat);
            chatroom[connect_table[to_handle].chat].mes.push_back(msg);

            map<int, user_t>::iterator it;
            for (it = connect_table.begin(); it != connect_table.end(); it++) {
                if(it->second.chat == connect_table[to_handle].chat)
                    send(it->first, pin_mes.c_str(), strlen(pin_mes.c_str()), 0);
            }
            chatroom[connect_table[to_handle].chat].mes.push_back(pin_mes);
        }

        else if(strcasecmp(buff, "/delete-pin\n") == 0){
            bool exist_pin = false;
            for (vector<string>::iterator it = chatroom[connect_table[to_handle].chat].mes.begin(); it != chatroom[connect_table[to_handle].chat].mes.end();) {
                if (it->compare(0, 3, "Pin") == 0) {
                    it = chatroom[connect_table[to_handle].chat].mes.erase(it, it + 2);
                    exist_pin = true;
                }
                else 
                    it++;
            }
            if(!exist_pin){
                sprintf(msg, "No pin message in chat room %d\n", connect_table[to_handle].chat);
                send(to_handle, msg, strlen(msg), 0);
            }
        }

        else if(strcasecmp(buff, "/exit-chat-room\n") == 0){
            sprintf(msg, "%s had left the chat room.\n", connect_table[to_handle].username.c_str());
            map<int, user_t>::iterator it;
            for (it = connect_table.begin(); it != connect_table.end(); it++) {
                if(it->second.chat == connect_table[to_handle].chat && it->first != to_handle)
                    send(it->first, msg, strlen(msg), 0);
            }
            name_list[connect_table[to_handle].username].chat = 0;
            connect_table[to_handle].chat = 0;

            char tmp[] = "% ";
            send(to_handle, tmp, strlen(tmp), 0);
        }

        else if(strcasecmp(buff, "/list-user\n") == 0){
            map<string, account>::iterator itt;
            for (itt = name_list.begin(); itt != name_list.end(); itt++) {
                if(itt->second.chat == connect_table[to_handle].chat){
                    sprintf(msg, "%s %s\n", itt->second.username, itt->second.status);
                    send(to_handle, msg, strlen(msg), 0);
                }
            }
        }

        else if(buff[0] == '/'){
            sprintf(msg, "Error: Unknown command\n");
            send(to_handle, msg, strlen(msg), 0);
        }

        else{
            sprintf(msg, "[%s]: ", connect_table[to_handle].username.c_str());
            broadcast(msg, strlen(msg), connect_table[to_handle].chat);
            chatroom[connect_table[to_handle].chat].mes.push_back(msg);

            broadcast(buff, strlen(buff), connect_table[to_handle].chat);
            chatroom[connect_table[to_handle].chat].mes.push_back(buff);
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("server usage: %s [port number]\n", argv[0]);
        return -1;
    }

    init();

    int reuse = 1;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(atoi(argv[1]));

    bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
    listen(listenfd, MAX_EVENTS);

    fd_set master_fds;
    FD_ZERO(&master_fds);
    FD_SET(listenfd, &master_fds);
    
    int max_fd = listenfd;

    while (1) {
        fd_set read_fds = master_fds;
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &read_fds)) {
                if (fd == listenfd) {
                    struct sockaddr_in cliaddr;
                    int clilen = sizeof(cliaddr);
                    int connfd = accept(listenfd, (struct sockaddr*) &cliaddr, (socklen_t*) &clilen);
                    max_fd = (connfd > max_fd) ? connfd : max_fd;
                    FD_SET(connfd, &master_fds);

                    user_t user_info;
                    user_info.cliaddr  = cliaddr;
                    user_info.login    = 0;
                    user_info.chat     = 0;
                    user_info.username = "temp-user";
                    connect_table[connfd] = user_info;

                    char wel[] = "*********************************\n** Welcome to the Chat server. **\n*********************************\n% ";
                    send(connfd, wel, strlen(wel), 0);
                } 
                else {
                    if (handle_read(fd) < 0) {
                        if(name_list[connect_table[fd].username].login == 1){
                            name_list[connect_table[fd].username].login = 0;
                            name_list[connect_table[fd].username].chat  = 0;
                            sprintf(name_list[connect_table[fd].username].status, "offline");
                        }
                        connect_table.erase(fd);
                        close(fd);
                        FD_CLR(fd, &master_fds);
                    }
                }
            }
        }
    }

    return 0;
}