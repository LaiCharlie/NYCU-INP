#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
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
#include <openssl/err.h>
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

void urlDecode(const char* url, char* decoded) {
    size_t len = strlen(url);
    size_t decoded_pos = 0;

    for (size_t i = 0; i < len; i++) {
        if (url[i] == '%' && i + 2 < len && isxdigit(url[i + 1]) && isxdigit(url[i + 2])) {
            char hex[3] = {url[i + 1], url[i + 2], '\0'};
            int value = (int)strtol(hex, NULL, 16);
            decoded[decoded_pos++] = (char)value;
            i += 2;
        } 
		else {
            decoded[decoded_pos++] = url[i];
        }
    }

    decoded[decoded_pos] = '\0';
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

    char file_end[128];
	strcpy(file_end, file_type(file_name));

	const char *last_dot = strrchr(file_end, '/');
	size_t len_to_keep = strlen(file_name);
    if (last_dot != NULL) {
		len_to_keep -= strlen(last_dot);
	}

	char dest[1024] = {0};
	strncpy(dest, file_name, len_to_keep);

    int file_fd = open(dest, O_RDONLY);
    if(file_fd == -1){
        snprintf(response, 4096, "HTTP/1.0 404 Not Found\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>\r\n");
        *response_len = strlen(response);
        return;
    }

	FILE* txt = fopen(file_name, "r");
	fseek(txt, 0, SEEK_END);
    long file_size = ftell(txt);
    rewind(txt);
	fclose(txt);

	char *header = (char *)malloc(4096 * sizeof(char));
    snprintf(header, 4096, "HTTP/1.0 200 OK\r\nContent-Length: %zu\r\nContent-Type: %s\r\n\r\n", file_size, type);
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
		char resp[] = "HTTP/1.0 501 Not Implemented\r\n\r\n";
		send(fd, resp, sizeof(resp), 0);
		close(fd);
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
    char buf[4096];
	int byteread = SSL_read(ssl, buf, sizeof(buf));
	if((strstr(buf, "GET") == NULL) || byteread <= 0){
		char resp[] = "HTTP/1.0 501 Not Implemented\r\n\r\n";
		SSL_write(ssl, resp, strlen(resp));
		close(fd);
    	SSL_free(ssl);
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
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    SSL_CTX* ssl_ctx = SSL_CTX_new(SSLv23_server_method());
    SSL_CTX_use_certificate_file(ssl_ctx, "/cert/server.crt", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ssl_ctx, "/cert/server.key", SSL_FILETYPE_PEM);
    SSL_CTX_set_cipher_list(ssl_ctx, "TLSv1.2");
    SSL_CTX_set_options(ssl_ctx, SSL_OP_SINGLE_DH_USE);

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