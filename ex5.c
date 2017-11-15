#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
	int fd[2];

	pipe(fd);

	if(fork()){ //parent process
		close(fd[0]);
		write(fd[1], "How are you?", 12);
	}
	else{
		char buf[100];
		close(fd[1]);
		read(fd[0], buf, 100);
		printf("Child received message : %s\n", buf);
		fflush(stdout);
	}
	exit(0);
}

