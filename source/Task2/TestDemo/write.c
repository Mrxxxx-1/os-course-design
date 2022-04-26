#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<fcntl.h>

#define BUFFER_SIZE 32
char inputBuf[32], outputBuf[32];
int main(){
	int fd, m, n;
	char c;
	fd = open("/dev/mydevice", O_RDWR);
	if(fd < 0){
		printf("open /dev/mydevice failed\n");
		exit(-1);
	}
	inputBuf[1] = '\0';
	while(1){
		printf("please enter a string,type \"exit\" to exit\n");
		gets(inputBuf);
		if(!strcmp(inputBuf,"exit"))
			break;
		inputBuf[strlen(inputBuf)+1]='\0';
		n = write(fd, inputBuf, strlen(inputBuf)*sizeof(char));
		printf(" %d words written\n",n);

	}
	close(fd);
	return 0;
}
