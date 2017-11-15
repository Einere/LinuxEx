#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#define BUFSIZE 256

int main(int argc, char *argv[]){
	int fd, nread;
	char buf[BUFSIZE];

	//if arguments is miss match
	if(argc != 2) fprintf(stderr, "usage : %s fifoname\n", argv[0]);

	//if failed to open FIFO
	if((fd = open(argv[1], O_RDONLY)) < 0){
		perror("failed to open FIFO ");
		return 0;
	}
	
	//roop until message is "quit"
	do{
		//if failed to read FIFO
		if((nread = read(fd, buf, BUFSIZE)) < 0) perror("failed to read ");

		//print message
		printf("%s",buf);

		//if message is "quit"
		if(strcmp(buf,"quit\n") == 0) break;

		//clear buf
		fflush(stdin);
	}while(nread > 0);
	
	return 0;
}

