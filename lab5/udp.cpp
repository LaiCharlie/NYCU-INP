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
    int sockfd;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);    // IPPROTO_UDP
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("140.113.213.213");
    servaddr.sin_port = htons(10301);

    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    // ---------- buffer --------------
    int sersiz = sizeof(struct sockaddr_in);
    char buf[32768];
    memset(buf,'\0',sizeof(buf));
    // int bytesRead = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&servaddr, &sersiz);
    // buf[bytesRead] = '\0';
    if(recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&servaddr, &sersiz) < 0)
        cout << "error recv\n";

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
    sendto(sockfd, ans.c_str(), strlen(ans.c_str()), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
    
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

/*
// #include <stdio.h>
// #include <fcntl.h>
// #include <dirent.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <sys/uio.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <openssl/ssl.h>

// #define errquit(m)	{ perror(m); exit(-1); }
// #define BUFFER_SIZE 8388608

// static int port_http = 80;
// static int port_https = 443;
// static const char *docroot = "/html";

// const char *file_type(const char *file_name){
//     const char *dot = strrchr(file_name, '.');
//     if (!dot || dot == file_name)
//         return "";
//     return dot + 1;
// }

// const char *get_file_name(const char *file_name){
//     const char *dot = strrchr(file_name, '/');
//     if (!dot || dot == file_name)
//         return "";
//     return dot + 1;
// }

// // get server response type
// const char *resp_type(const char *file_ext){
//     if(strcasecmp(file_ext, "html") == 0)
//         return "text/html";
// 	else if(strcasecmp(file_ext, "jpg") == 0)
//         return "image/jpeg";
// 	else if(strcasecmp(file_ext, "png") == 0)
//         return "image/png";
// 	else if(strcasecmp(file_ext, "mp3") == 0)
//         return "audio/mpeg";
// 	else if(strcasecmp(file_ext, "txt") == 0)
//         return "text/plain;charset=utf-8";
// 	else
//         return "text/html";
// }


// void url_decode(char *str) {
//     char *p = str, *q = str;
//     while(*p){
//         if(*p == '%' && p[1] && p[2]){
//             *q++ = (char)strtol((p+1), NULL, 16);
//             p += 3;
//         }
// 		else
//             *q++ = *p++;
//     }
//     *q = '\0';
// }

// void http_resp(int fd, const char *file_name, const char *file_ext, char *response, size_t *response_len){
//     const char *type = resp_type(file_ext);

// 	if(strlen(file_type(file_name)) == 0 && file_name[strlen(file_name)-1] != '/'){
// 		int file_fd = open(file_name, O_RDONLY);
// 		if(file_fd == -1)
// 			snprintf(response, 4096, "HTTP/1.0 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>\r\n");
// 		else
// 			snprintf(response, 4096, "HTTP/1.0 301 Moved Permanently\r\nLocation: /%s/\r\n\r\n", get_file_name(file_name));
//         *response_len = strlen(response);
// 		close(file_fd);
// 		write(fd, response, strlen(response));
//         return;
// 	}
// 	else if(strlen(file_type(file_name)) == 0 && file_name[strlen(file_name)-1] == '/'){
// 		int file_fd = open(file_name, O_RDONLY);
// 		if(file_fd == -1)
// 			snprintf(response, 4096, "HTTP/1.0 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>\r\n");
// 		else
// 			snprintf(response, 4096, "HTTP/1.0 403 Forbidden\r\n\r\n<html><body><h1>403 Forbidden</h1></body></html>\r\n");
//         *response_len = strlen(response);
// 		close(file_fd);
// 		write(fd, response, strlen(response));
//         return;
// 	}

// 	// -------------------------------------

//     int file_fd = open(file_name, O_RDONLY);
//     if(file_fd == -1){
//         snprintf(response, 4096, "HTTP/1.0 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>\r\n");
//         *response_len = strlen(response);
// 		write(fd, response, strlen(response));
//         return;
//     }
//     close(file_fd);

// 	char *header = (char *)malloc(4096 * sizeof(char));
//     snprintf(header, 4096, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", type);
// 	write(fd, header, strlen(header));

//     FILE* txt = fopen(file_name, "r");
//     char buf[65536];
// 	size_t bytes_read;
//     while((bytes_read = fread(buf, 1, sizeof(buf), txt)) > 0){
//         write(fd, buf, bytes_read);
// 	}
//     fclose(txt);
	
//     free(header);
// }

// void* handle_client(void* cfd){
//     int fd = (int)cfd;
// 	char buf[2048];
// 	memset(buf, '\0', sizeof(buf));
// 	int byteread = recv(fd, buf, sizeof(buf), 0);
// 	if(byteread <= 0 || strstr(buf, "GET") == NULL){
// 		char resp[] = "HTTP/1.0 501 Not Implemented\r\n\r\n<html><body><h1>501 Not Implemented</h1></body></html>\r\n";
// 		send(fd, resp, sizeof(resp), 0);
//         close(fd);
// 		return NULL;
// 	}

// 	char filename[2048];
// 	sscanf(buf, "GET %s", filename);
// 	if(strlen(filename) == 1 || filename[1] == '?')
// 		strcpy(filename, "/index.html");

// 	url_decode(filename);

//     char full_path[2048];
// 	snprintf(full_path, sizeof(full_path), "%s%s", docroot, filename);

//     char filetype[128];
// 	strcpy(filetype, file_type(full_path));

// 	char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
//     size_t response_len;
//     http_resp(fd, full_path, filetype, response, &response_len);


// 	free(response);
//     close(fd);
// 	return NULL;
// }

// int main(int argc, char *argv[]) {
// 	int s;
// 	struct sockaddr_in sin;

// 	if(argc > 1) { port_http  = strtol(argv[1], NULL, 0); }
// 	if(argc > 2) { if((docroot = strdup(argv[2])) == NULL) errquit("strdup"); }
// 	if(argc > 3) { port_https = strtol(argv[3], NULL, 0); }

// 	if((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) errquit("socket");

// 	do {
// 		int v = 1;
// 		setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
// 	} while(0);

// 	bzero(&sin, sizeof(sin));
// 	sin.sin_family = AF_INET;
// 	sin.sin_port = htons(80);
// 	if(bind(s, (struct sockaddr*) &sin, sizeof(sin)) < 0) errquit("bind");
// 	// if(listen(s, SOMAXCONN) < 0) errquit("listen");
// 	if(listen(s, 1200) < 0) errquit("listen");

// 	do {
// 		int c;
// 		struct sockaddr_in csin;
// 		socklen_t csinlen = sizeof(csin);

// 		if((c = accept(s, (struct sockaddr*) &csin, &csinlen)) < 0) {
// 			perror("accept");
// 			continue;
// 		}

//         pthread_t tid;
//         if(pthread_create(&tid, NULL, handle_client, (void*)c) < 0){
//             perror("pthread_create run is error!");
//         }
// 		pthread_detach(tid);
		
// 	} while(1);

//     close(s);
// 	return 0;
// }

#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>

#define errquit(m)	{ perror(m); exit(-1); }
#define BUFFER_SIZE 8388608

static int port_http = 80;
static int port_https = 443;
static const char *docroot = "/html";

const char *file_type(const char *file_name){
    const char *dot = strrchr(file_name, '.');
    if (!dot || dot == file_name)
        return "";
    return dot + 1;
}

const char *get_file_name(const char *file_name){
    const char *dot = strrchr(file_name, '/');
    if (!dot || dot == file_name)
        return "";
    return dot + 1;
}

// get server response type
const char *resp_type(const char *file_ext){
    if(strcasecmp(file_ext, "html") == 0)
        return "text/html";
	else if(strcasecmp(file_ext, "jpg") == 0)
        return "image/jpeg";
	else if(strcasecmp(file_ext, "png") == 0)
        return "image/png";
	else if(strcasecmp(file_ext, "mp3") == 0)
        return "audio/mpeg";
	else if(strcasecmp(file_ext, "txt") == 0)
        return "text/plain;charset=utf-8";
	else
        return "text/html";
}


void url_decode(char *str) {
    char *p = str, *q = str;
    while(*p){
        if(*p == '%' && p[1] && p[2]){
            *q++ = (char)strtol((p+1), NULL, 16);
            p += 3;
        }
		else
            *q++ = *p++;
    }
    *q = '\0';
}

void http_resp(int fd, const char *file_name, const char *file_ext, char *response, size_t *response_len){
    const char *type = resp_type(file_ext);
	if(strlen(file_type(file_name)) == 0 && file_name[strlen(file_name)-1] != '/'){
		int file_fd = open(file_name, O_RDONLY);
		if(file_fd == -1)
			snprintf(response, 4096, "HTTP/1.0 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>\r\n");
		else
			snprintf(response, 4096, "HTTP/1.0 301 Moved Permanently\r\nLocation: /%s/\r\n\r\n", get_file_name(file_name));
        *response_len = strlen(response);
		close(file_fd);
        return;
	}
	else if(strlen(file_type(file_name)) == 0 && file_name[strlen(file_name)-1] == '/'){
		int file_fd = open(file_name, O_RDONLY);
		if(file_fd == -1)
			snprintf(response, 4096, "HTTP/1.0 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>\r\n");
		else
			snprintf(response, 4096, "HTTP/1.0 403 Forbidden\r\n\r\n<html><body><h1>403 Forbidden</h1></body></html>\r\n");
        *response_len = strlen(response);
		close(file_fd);
        return;
	}

	// -------------------------------------

    int file_fd = open(file_name, O_RDONLY);
    if(file_fd == -1){
        snprintf(response, 4096, "HTTP/1.0 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>\r\n");
        *response_len = strlen(response);
        return;
    }

    char *header = (char *)malloc(4096 * sizeof(char));
    snprintf(header, 4096, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", type);
    *response_len = 0;
    memcpy(response, header, strlen(header));
    *response_len += strlen(header);
    free(header);

    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, response + *response_len, BUFFER_SIZE - *response_len)) > 0) {
        *response_len += bytes_read;
    }
	
    close(file_fd);
}

void handle_client(int fd){
	char buf[2048];
	memset(buf, '\0', sizeof(buf));
	int byteread = recv(fd, buf, sizeof(buf), 0);
	if(byteread <= 0 || strstr(buf, "GET") == NULL){
		char resp[] = "HTTP/1.0 501 Not Implemented\r\n\r\n<html><body><h1>501 Not Implemented</h1></body></html>\r\n";
		send(fd, resp, sizeof(resp), 0);
		return;
	}

	char filename[2048];
	sscanf(buf, "GET %s", filename);
	if(strlen(filename) == 1 || filename[1] == '?')
		strcpy(filename, "/index.html");

	url_decode(filename);

    char full_path[2048];
	snprintf(full_path, sizeof(full_path), "%s%s", docroot, filename);

    char filetype[128];
	strcpy(filetype, file_type(full_path));

	char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    size_t response_len;
    http_resp(fd, full_path, filetype, response, &response_len);
	send(fd, response, response_len, 0);

	free(response);
    close(fd);
	return;
}

void handle_https(int fd, SSL *ssl){
    char buf[2048];
	memset(buf, '\0', sizeof(buf));
	int byteread = SSL_read(ssl, buf, sizeof(buf));
	if(byteread <= 0 || strstr(buf, "GET") == NULL){
		char resp[] = "HTTP/1.0 501 Not Implemented\r\n\r\n<html><body><h1>501 Not Implemented</h1></body></html>\r\n";
		SSL_write(ssl, resp, sizeof(resp));
		return;
	}

	char filename[2048];
	sscanf(buf, "GET %s", filename);
	if(strlen(filename) == 1 || filename[1] == '?')
		strcpy(filename, "/index.html");

	url_decode(filename);

    char full_path[2048];
	snprintf(full_path, sizeof(full_path), "%s%s", docroot, filename);

    char filetype[128];
	strcpy(filetype, file_type(full_path));

	char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    size_t response_len;
    http_resp(fd, full_path, filetype, response, &response_len);
	SSL_write(ssl, response, response_len);

	free(response);
    close(fd);
    SSL_free(ssl);
	return;
}

int main(int argc, char *argv[]) {

	if(argc > 1) { port_http  = strtol(argv[1], NULL, 0); }
	if(argc > 2) { if((docroot = strdup(argv[2])) == NULL) errquit("strdup"); }
	if(argc > 3) { port_https = strtol(argv[3], NULL, 0); }

	int s1, s2;
	struct sockaddr_in sin1, sin2;
	if((s1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) errquit("socket");
    if((s2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) errquit("socket");

	do {
		int v = 1;
		setsockopt(s1, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
	} while(0);

    do {
		int v = 1;
		setsockopt(s2, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
	} while(0);

	bzero(&sin1, sizeof(sin1));
	sin1.sin_family = AF_INET;
	sin1.sin_port = htons(port_http);
	if(bind(s1, (struct sockaddr*) &sin1, sizeof(sin1)) < 0) errquit("bind");
	if(listen(s1, 1200) < 0) errquit("listen");

    bzero(&sin2, sizeof(sin2));
	sin2.sin_family = AF_INET;
	sin2.sin_port = htons(port_https);
	if(bind(s2, (struct sockaddr*) &sin2, sizeof(sin2)) < 0) errquit("bind");
	if(listen(s2, 1200) < 0) errquit("listen");
	
	SSL_library_init(); 
    SSL_CTX* ssl_ctx = SSL_CTX_new(SSLv23_server_method());
	SSL_CTX_set_ecdh_auto(ssl_ctx, 1);
    SSL_CTX_use_certificate_file(ssl_ctx, "/cert/server.crt", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ssl_ctx, "/cert/server.key", SSL_FILETYPE_PEM);

    int epoll_fd = epoll_create1(0);

    struct epoll_event http_event;
    http_event.events = EPOLLIN;
    http_event.data.fd = s1;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, s1, &http_event);

    struct epoll_event https_event;
    https_event.events = EPOLLIN;
    https_event.data.fd = s2;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, s2, &https_event);

	do {
        struct epoll_event events[10];
        int num_events = epoll_wait(epoll_fd, events, 10, -1);
        if (num_events == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i < num_events; i++){
            if(events[i].data.fd == s1){
                int c;
                struct sockaddr_in csin;
                socklen_t csinlen = sizeof(csin);

                if((c = accept(s1, (struct sockaddr*) &csin, &csinlen)) < 0) {
                    perror("accept");
                    continue;
                }

                handle_client(c);
            }
            if(events[i].data.fd == s2){
                int c;
                struct sockaddr_in csin;
                socklen_t csinlen = sizeof(csin);

                if((c = accept(s2, (struct sockaddr*) &csin, &csinlen)) < 0) {
                    perror("accept");
                    continue;
                }

                SSL *ssl = SSL_new(ssl_ctx);
                SSL_set_fd(ssl, c);
                SSL_accept(ssl);

                handle_https(c, ssl);
            }
        }
	} while(1);

    close(s1);
    close(s2);
    close(epoll_fd);
    SSL_CTX_free(ssl_ctx);

	return 0;
}
*/