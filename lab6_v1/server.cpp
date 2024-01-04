#include <iostream>
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
	struct file_no temp, next;
	next.fileno = -1;

	struct timeval start, end;
	char content[25][CHUNK_SIZE];
	bool file_recv[25];
	int  file_leng[25];
	int  filenum = 0;
	int  rlen;

	while(true){
		gettimeofday(&start, NULL);
		int totidx = 0;
		memset(file_recv, false, sizeof(file_recv));

		while((rlen = recvfrom(s, (void*)&temp, sizeof(temp), 0, (struct sockaddr*) &csin, &csinlen)) > 0){
			if(filenum < temp.fileno) { break;} // skip = true;
			int idx = temp.start / CHUNK_SIZE;
			if(file_recv[idx]) continue;
			
			memcpy(content[idx], temp.data, temp.end-temp.start);
			file_recv[idx] = true;
			file_leng[idx] = temp.end - temp.start;
			totidx = temp.filesz / CHUNK_SIZE + 1;

			bool ex = true;
			for(int i=0;i<totidx;i++)
				ex = ex & file_recv[i];
			if(ex) break;
		}

		while((rlen = recvfrom(s, (void*)&next, sizeof(next), 0, (struct sockaddr*) &csin, &csinlen)) > 0){
			if(filenum != next.fileno) break;
			if(filenum == 999) break;
		}

		int filefd = open(file_name(path, filenum).c_str(), O_WRONLY | O_CREAT);
		for(int i=0;i<totidx;i++){
			if(file_recv[i]) write(filefd, content[i], file_leng[i]);
			else cout << "file " << filenum << " (" << file_name(path, filenum) << ") lost packet No." << i << '\n';
		}
		close(filefd);

        gettimeofday(&end, NULL);
		double delay = ((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec) / 1000.0;
		cout << "now writing file " << filenum << " (" << file_name(path, filenum) << ") | spend " << delay << " ms\n";

		filenum++;
		if(filenum >= total) break;
	}

	close(s);
}