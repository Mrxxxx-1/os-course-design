#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<fcntl.h>

#define BUFFER_SIZE 32
char inputBuf[32], outputBuf[32];
int main(){
	int fd, m, n;
	fd = open("/dev/mydevice", O_RDWR|O_NONBLOCK);
	if(fd < 0){
		printf("open /dev/mydevice failed\n");
		exit(-1);
	}
	while(1){
		sleep(1);
		m = read(fd, outputBuf, 1 * sizeof(char));
		if(m < 0 || outputBuf[0]<0){
			printf("no character!\n");
			continue;
		
		}
		else{
			printf("read char = %c \n", outputBuf[0], outputBuf[0]);
		}
	}
	close(fd);
	return 0;
}
