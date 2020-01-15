#include <stdint.h>
#include <unistd.h> 	
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define BUF_LEN 64*1024
#define TIMES 300 
#define FILE_NAME "test.bin"

static char *dev_name = "/dev/gti2800-0";

static uint64_t time_in_ns()
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return (uint64_t) tp.tv_sec * (1000ULL * 1000ULL * 1000ULL) +
	                  tp.tv_nsec;
}

int main(int argc,char *argv[])
{
	int fd;
	uint64_t start, end, use;
	FILE *f_in;
	ssize_t r_num, ret;

	fd = open(dev_name, O_RDWR);  // Read/Write
	if (fd == -1) {
		printf("open failed.\n");
		return EXIT_FAILURE;
	}

	char *buf = (char *) malloc(BUF_LEN);
	if (NULL == buf) {
		printf("malloc error\n");
		return EXIT_FAILURE;
	}

	f_in = fopen(FILE_NAME,"rb");  // Read only binary file
	if(!f_in){  // Open file failed.
		printf("open file:%s error\n", FILE_NAME);
	}else{
		start = time_in_ns();
		while(!feof(f_in)) { 
			r_num = fread(buf, sizeof(char), BUF_LEN, f_in);
			// printf("r_num:%d\n",(int) r_num);

			ret = write(fd, (const void *) buf, r_num);
			if (ret != r_num) {	
				printf("write buf to driver failed, return val:%d \n", (int)ret);
				break;
			}
		}

		fclose(f_in);
	}

	ret = read(fd, &r_num, sizeof(int));
	printf("Total write: %dbytes\n", (int)ret);
	/*
	if(ret != sizeof(int)){
		printf("read fails, errno : %s(%d) \n", strerror(errno), errno);
	}else{
		printf("read data: %d\n", (int)ret);
	}
	*/

	close(fd);
	end = time_in_ns();
	use = end - start;
	printf("Test time:%3llu.%09llus\n", 
		(unsigned long long) use / 1000000000, 
		(unsigned long long) use % 1000000000);

	return EXIT_SUCCESS;
}
