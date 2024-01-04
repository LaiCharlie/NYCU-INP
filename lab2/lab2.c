#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 	//read write lseek
#include <stdint.h> 	//uint8_t uint16_t uint32_t
#include <arpa/inet.h> 	//htons honl ntohs ntohl
#include <fcntl.h> 	    //open

typedef struct {
    uint64_t magic;     /* 'BINFLAG\x00' */
    uint32_t datasize;  /* in big-endian */
    uint16_t n_blocks;  /* in big-endian */
    uint16_t zeros;
} __attribute((packed)) binflag_header_t;

typedef struct {
    uint32_t offset;        /* in big-endian */
    uint16_t cksum;         /* XOR'ed results of each 2-byte unit in payload */
    uint16_t length;        /* ranges from 1KB - 3KB, in big-endian */
    uint8_t  payload[0];
} __attribute((packed)) block_t;

typedef struct {
   uint16_t length;        /* length of the offset array, in big-endian */
   uint32_t offset[0];     /* offset of the flags, in big-endian */
} __attribute((packed)) flag_t;

int main(int argc, char *argv[]) {
	const char* srcfile = "demo2.bin";

	int srcfd = open(srcfile, O_RDONLY);
	
	binflag_header_t h;
	read(srcfd, &h, sizeof(h));
	h.datasize = htonl(h.datasize);
	h.n_blocks = htons(h.n_blocks);
	
	// printf("The number of blocks: %d\n", h.n_blocks);
	// printf("The number of datasz: %d\n", h.datasize);

	char pl[8000000];
	memset(pl,'\0',sizeof(pl));
	uint32_t now = 0;

    for(int i = 0; i < h.n_blocks; i++) {
        block_t fileBlock;
        read(srcfd, &fileBlock, sizeof(fileBlock));
        fileBlock.cksum  = htons(fileBlock.cksum);
        fileBlock.offset = htonl(fileBlock.offset);
        fileBlock.length = htons(fileBlock.length);

        // printf("Block No.%d\n", i);
		// printf("The number of offset: %d\n", fileBlock.offset);
		// printf("The number of length: %d\n\n", fileBlock.length);

        uint8_t* payload = malloc(fileBlock.length);
        read(srcfd, payload, fileBlock.length);

        uint16_t temp = (payload[0] << 8) + payload[1];
        for(int j = 2; j < fileBlock.length; j += 2) {
            uint16_t tmp = (payload[j] << 8) + payload[j + 1];
            temp = temp ^ tmp;
        }

        if(temp != fileBlock.cksum) {
            // printf("Block No. %d checksum error\n", i);
            free(payload);
            continue;
        }

        // Process the payload data as needed
        now = fileBlock.offset;
        for(int j = 0; j < fileBlock.length; j++) {
            uint8_t tmp = payload[j];
			char u,d;
			if(tmp/16 < 10)
				u = 48 + tmp/16;
			else
				u = 87 + tmp/16;
			if(tmp%16 < 10)
				d = 48 + tmp%16;
			else
				d = 87 + tmp%16;
			pl[now*2]   = u;
			pl[now*2+1] = d;
			now++;

            // printf("%c%c ",u,d);
        }
        // printf("\n\n");
        free(payload);
    }

	flag_t flag;
	read(srcfd, &flag, sizeof(flag));
	flag.length = htons(flag.length);

	read(srcfd, flag.offset, (flag.length) * sizeof(uint32_t));
	for(int i=0;i<flag.length;i++){
		flag.offset[i] = htonl(flag.offset[i]);
		printf("%c%c%c%c", pl[flag.offset[i]*2], pl[flag.offset[i]*2+1], pl[flag.offset[i]*2+2], pl[flag.offset[i]*2+3]);
        // printf("  ");
	}
	printf("\n");

	return 0;
}
