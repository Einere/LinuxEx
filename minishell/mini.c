#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "getch.h"

#define BUFSIZE 512

int main(int argc, char* argv[]){
	fprintf(stderr, "User Shell >> "); 
	char cmd[BUFSIZE];
	memset(cmd, 0, sizeof(cmd));
	int i = 0;

	while( (cmd[i] = getch()) != '\n'){
		i++;
	}
	
	for(i = 0; i < sizeof(cmd); i++){
		printf("%c", cmd[i]);
	}
	
	int c_pid;
	int status;

	if((c_pid = fork()) < 0){
		fprintf(stderr, "fork failed\n");
	}
	else if(c_pid > 0){
		char argv[] = {NULL};
		execvp(cmd, argv);
		fprintf(stderr, "child process failed\n");
		return 1;
	}
	else if(c_pid > 0){
		c_pid = waitpid(c_pid, &status, WNOHANG);
	}


	return 0;
}
