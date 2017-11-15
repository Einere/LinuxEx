#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUFSIZE 256

int main(int argc, char *argv[]){
	int fd, nwrite;
	char buf[BUFSIZE];

	//if argument number is miss match
	if(argc != 2 ) fprintf(stderr, "usage : %s fifoname\n", argv[0]);
	
	//if failed to open FIFO file
	if((fd = open(argv[1], O_WRONLY)) < 0){
		perror("failed to open FIFO ");
		return 0;
	}

	//input stdin string to buf until string is "quit"
	while(fgets(buf, BUFSIZE, stdin)){
		//input message and save at buf
		if((nwrite = write(fd, buf, BUFSIZE)) < 0) perror("failed to write to FIFO ");
		
		//if message is quit
		if(strcmp(buf,"quit\n") == 0) break;
		
		//clear buf
		fflush(stdin);
	}

	return 0;
}

