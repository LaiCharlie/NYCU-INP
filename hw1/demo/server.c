#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/uio.h>
#include <sys/stat.h>
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
		write(fd, response, strlen(response));
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
		write(fd, response, strlen(response));
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
		write(fd, response, strlen(response));
        return;
    }
    close(file_fd);

    FILE* txt = fopen(dest, "r");
	fseek(txt, 0, SEEK_END);
    long file_size = ftell(txt);
    rewind(txt);

	char *header = (char *)malloc(4096 * sizeof(char));
    snprintf(header, 4096, "HTTP/1.0 200 OK\r\nContent-Length: %zu\r\nContent-Type: %s\r\n\r\n", file_size, type);
	write(fd, header, strlen(header));
    free(header);

    char buf[65536];
	size_t bytes_read;
    while((bytes_read = fread(buf, 1, sizeof(buf), txt)) > 0){
        write(fd, buf, bytes_read);
	}
    fclose(txt);
}

void* handle_client(void* cfd){
    int fd = (int)cfd;
	char buf[2048];
	memset(buf, '\0', sizeof(buf));
	int byteread = recv(fd, buf, sizeof(buf), 0);
	if(byteread <= 0 || strstr(buf, "GET") == NULL){
		char resp[] = "HTTP/1.0 501 Not Implemented\r\n\r\n<html><body><h1>501 Not Implemented</h1></body></html>\r\n";
		send(fd, resp, sizeof(resp), 0);
        close(fd);
		return NULL;
	}

	char filename1[2048];
	char filename[2048];
	sscanf(buf, "GET %s", filename1);
	if(strlen(filename1) == 1 || filename1[1] == '?')
		strcpy(filename1, "/index.html");

	urlDecode(filename1, filename);

    char full_path[2048];
	snprintf(full_path, sizeof(full_path), "%s%s", docroot, filename);

    char filetype[128];
	strcpy(filetype, file_type(full_path));

	char *response = (char *)malloc(BUFFER_SIZE * sizeof(char));
    size_t response_len;
    http_resp(fd, full_path, filetype, response, &response_len);
	free(response);

    close(fd);
	return NULL;
}

int main(int argc, char *argv[]) {
	int s;
	struct sockaddr_in sin;

	if(argc > 1) { port_http  = strtol(argv[1], NULL, 0); }
	if(argc > 2) { if((docroot = strdup(argv[2])) == NULL) errquit("strdup"); }
	if(argc > 3) { port_https = strtol(argv[3], NULL, 0); }

	if((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) errquit("socket");

	do {
		int v = 1;
		setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
	} while(0);

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(80);
	if(bind(s, (struct sockaddr*) &sin, sizeof(sin)) < 0) errquit("bind");
	if(listen(s, 1200) < 0) errquit("listen");

	do {
		int c;
		struct sockaddr_in csin;
		socklen_t csinlen = sizeof(csin);

		if((c = accept(s, (struct sockaddr*) &csin, &csinlen)) < 0) {
			perror("accept");
			continue;
		}

        pthread_t tid;
        if(pthread_create(&tid, NULL, handle_client, (void*)c) < 0){
            perror("pthread_create run is error!");
        }
		pthread_detach(tid);
		
	} while(1);

    close(s);
	return 0;
}