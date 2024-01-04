#include <iostream>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

#define err_quit(m) { perror(m); exit(-1); }

const int CHUNK_SIZE = 1460;

struct file_no{
	uint16_t fileno;
	uint16_t filesz;
	uint16_t start;
	uint16_t end;

	char data[CHUNK_SIZE];
};

string file_name(string path, int num){
	string str = "000000";
	int i = 5;
	while(num){
		if(num%10) str[i] = '0' + (num%10);
		num /= 10;
		i--;
	}
	return path + "/" + str;
}

int main(int argc, char *argv[]) {
	if(argc < 5) {
		return -fprintf(stderr, "usage: %s <path> <num> <port> <ip>\n", argv[0]);
	}

	setvbuf(stdin,  NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(strtol(argv[3], NULL, 0));
	if(inet_pton(AF_INET, argv[4], &sin.sin_addr) != 1) {
		return -fprintf(stderr, "** cannot convert IPv4 address for %s\n", argv[1]);
	}

	int s = -1;
	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		err_quit("socket");

	string   path  = argv[1];
    uint32_t total = (uint32_t)atoi(argv[2]);
    uint16_t port  = (uint16_t)atoi(argv[3]);
    string   ip    = argv[4];

	// freopen("out.txt", "w", stdout);
	for(int i = 0; i < total; i++){
		int filefd = open(file_name(path, i).c_str(), O_RDONLY);
		if(filefd == -1){
			cout << "file open error\n";
			break;
		}

		auto  size = (uint32_t)lseek(filefd, 0, SEEK_END);
        auto  data = new char[size];
        lseek(filefd, 0, SEEK_SET);
        read(filefd, data, size);
		close(filefd);

		struct sockaddr_in csin;
		socklen_t csinlen = sizeof(csin);
        int offset = 0;

		struct file_no temp;
		temp.fileno = i;
		temp.filesz = size;

        while (offset < size) {
            int remaining = size - offset;
            int chunk_size = min(remaining, CHUNK_SIZE);
			temp.start = offset;
			temp.end   = offset + chunk_size;
			memset(temp.data, '0', sizeof(temp.data));
			memcpy(temp.data, data + offset, chunk_size);

			for(int j = 0; j < 10; j++){
            	sendto(s, (void*)&temp, sizeof(temp), 0, (struct sockaddr*) &sin, sizeof(sin));
				usleep(300);
			}
            offset += chunk_size;
			usleep(1000);
        }
		// 30 times 100 -> 500 => 121 sec

		delete(data);
		usleep(1000);
	}

	usleep(2000000);
	close(s);
}

// TOKEN=pjfEJMNjEo9JNmf4 python3 submit.py server client
