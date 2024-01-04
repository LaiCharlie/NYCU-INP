#include <iostream>
#include <bitset>
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
	uint16_t totidx;
	uint16_t pktsz;
	uint16_t idx;

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

	socklen_t sinlen = sizeof(sin);

	int s = -1;
	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		err_quit("socket");

	struct timeval timeout;
	timeout.tv_sec  = 0;
	timeout.tv_usec = 500 * 1000;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

	string   path  = argv[1];
    uint32_t total = (uint32_t)atoi(argv[2]);
    uint16_t port  = (uint16_t)atoi(argv[3]);
    string   ip    = argv[4];

	char content[100][25][CHUNK_SIZE];
	int  file_leng[100][25];
	int  idx[100];

	for(int j = 0; j < 10; j++){

		memset(content, '\0',   sizeof(content));
		memset(file_leng,  0, sizeof(file_leng));
		memset(idx, 0, sizeof(idx));

		for(int i = 0; i < 100; i++){
			int filefd = open(file_name(path, (j*100+i)).c_str(), O_RDONLY);

			auto  size = (uint32_t)lseek(filefd, 0, SEEK_END);
			auto  data = new char[size];
			lseek(filefd, 0, SEEK_SET);
			read(filefd, data, size);
			close(filefd);

			int offset = 0, num = 0;
			while (offset < size) {
				int remaining = size - offset;
				int chunk_size = min(remaining, CHUNK_SIZE);
				memcpy(content[i][num], (data + offset), chunk_size);
				file_leng[i][num] = chunk_size;
				offset += chunk_size;
				num++;
			}
			idx[i] = num;
			delete(data);
		}

		for(int i = 0; i < 100; i++){
			struct file_no temp;
			temp.fileno = j*100 + i;
			temp.totidx = idx[i];

			for(int k = 0; k < idx[i]; k++){
				temp.idx   = k;
				temp.pktsz = file_leng[i][k];
				// for(int sst = 0; sst < 5; sst++){
					sendto(s, (void*)&temp, sizeof(temp), 0, (struct sockaddr*) &sin, sizeof(sin));
					usleep(500);
				// }
			}
		}

		bitset<3000> check;
		while(true){
			bool fin_file = true;
			int rlen = recvfrom(s, (void*)&check, sizeof(check), 0, (struct sockaddr*) &sin, &sinlen);
			if(rlen < 0) continue;

			struct file_no temp;
			for(int n = 0; n < 100; n++){
				for(int m = 0; m < idx[n]; m++){
					if(!check[n*25 + m]){
						fin_file = false;
						int temp_file = n;
						int temp_idx  = m;

						temp.fileno = n + 100 * j;
						temp.totidx = idx[n];
						temp.pktsz  = file_leng[n][m];
						temp.idx    = m;
						memcpy(temp.data, content[n][m], file_leng[n][m]);

						for(int sst = 0; sst < 5; sst++){
							sendto(s, (void*)&temp, sizeof(temp), 0, (struct sockaddr*) &sin, sizeof(sin));
							usleep(500);
						}
					}
				}
			}
			if(fin_file) break;
		}

		// int rlen;
		// while((rlen = recvfrom(s, (void*)&check, sizeof(check), 0, (struct sockaddr*) &sin, &sinlen)) > 0)
		// 	continue;
	}

	// sleep(5);

	// usleep(1000000);
	close(s);
}

