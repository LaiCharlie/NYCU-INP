#include <iostream>
#include <bitset>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

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
	if(argc < 4) {
		return -fprintf(stderr, "usage: %s <path> <num> <port>\n", argv[0]);
	}

	setvbuf(stdin,  NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(strtol(argv[3], NULL, 0));

	int s;
	if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) err_quit("socket");
	bind(s, (struct sockaddr*) &sin, sizeof(sin));

	string   path  = argv[1];
    uint32_t total = (uint32_t)atoi(argv[2]);
    uint16_t port  = (uint16_t)atoi(argv[3]);

	struct sockaddr_in csin;
	socklen_t csinlen = sizeof(csin);

	struct timeval timeout;
	timeout.tv_sec  = 0;
	timeout.tv_usec = 500 * 1000;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

	struct timeval start, end;
	char content[100][25][CHUNK_SIZE];
	bool file_recv[100][25];
	int  file_leng[100][25];
	int  idx[100];
	bitset<3000> check;
	int  rlen;

	for(int z = 0; z < 10; z++){

		for(int p = 0; p < 100; p++){
			memset(file_recv[p], false, sizeof(file_recv[p]));
			memset(file_leng[p],     0, sizeof(file_leng[p]));
			for(int q = 0; q < 25; q++)
				memset(content[p][q], '\0', sizeof(content[p][q]));
		}
		memset(idx, 0, sizeof(idx));
		check.reset();
		// if(z) sleep(3);

		gettimeofday(&start, NULL);
		while(true){
			struct file_no temp;
			memset(temp.data, '\0', CHUNK_SIZE);
			bool outofb = false;
			while((rlen = recvfrom(s, (void*)&temp, sizeof(temp), 0, (struct sockaddr*) &csin, &csinlen)) > 0){
				if(temp.fileno < z * 100 || temp.fileno >= (z*100+100)){
					cout << temp.fileno << " out of bound\n";
					outofb = true;
					break;
				}
				// if(file_recv[temp.fileno%100][temp.idx]) continue;
				int tmpidx1 = temp.fileno%100;
				int tmpidx2 = temp.idx;

				int dup = 0;
				for(int i = 0; i < temp.pktsz; i++) 
					if(int(temp.data[i]) == 0) dup++;
				if(dup * 2 >= temp.pktsz) {
					// cout << "dup file " << temp.fileno << " | " << temp.totidx << "-" << temp.idx << " | pkt size = " << temp.pktsz << '\n';
					continue;
				}
				// cout << "file " << temp.fileno << " | arr idx = " << tmpidx1 << " | " << temp.totidx << "-" << temp.idx << " | pkt size = " << temp.pktsz << '\n';

				memcpy(content[tmpidx1][tmpidx2], temp.data, temp.pktsz);
				file_recv[tmpidx1][tmpidx2] = true;
				file_leng[tmpidx1][tmpidx2] = temp.pktsz;
				idx[tmpidx1]                = temp.totidx;
			}

			for(int i = 0; i < 100; i++){
				for(int j = 0; j < max(20, idx[i]); j++){
					if(file_recv[i][j] == false) check.set(i*25 + j, 0);
					else check.set(i*25 + j);
				}
			}

			for(int bs = 0; bs < 3; bs++){
				// cout << "send bitset\n";
				sendto(s, (void*)&check, sizeof(check), 0, (struct sockaddr*) &csin, sizeof(csin));
				usleep(100);
			}

			bool fini = true;
			for(int i = 0; i < 100; i++){
				for(int j = 0; j < 25; j++){
					if(idx[i] == 0 || (file_recv[i][j] == false && j < idx[i])) fini = false;
				}
			}
			if(fini || outofb){ 
				for(int bs = 0; bs < 5; bs++){
					// cout << "send bitset\n";
					sendto(s, (void*)&check, sizeof(check), 0, (struct sockaddr*) &csin, sizeof(csin));
					usleep(100);
				}
				break;
			}
		}

		for(int i = 0; i < 100; i++){
			// cout << "writing " << file_name(path, (i + z*100)) << '\n';
			int filefd = open(file_name(path, (i + z*100)).c_str(), O_WRONLY | O_CREAT);
			for(int j = 0; j < idx[i]; j++){
				write(filefd, content[i][j], file_leng[i][j]);
			}
			close(filefd);
		}

		gettimeofday(&end, NULL);
		double delay = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000.0; // ms
		cout << "write " << (z*100) << " ~ " << (z*100 + 99) << " files | spend " << delay << " ms\n";
	}

	close(s);
}

// gettimeofday(&start, NULL);
// gettimeofday(&end, NULL);
// double delay = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000.0; // ms