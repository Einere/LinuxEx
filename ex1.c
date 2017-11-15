#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv){
	int i, n, c_status;
	
	printf("enter the number of son process\n");
	scanf("%d",&n);
	
	pid_t pid[n];

	for(i = 0; i < n; i++){
		if((pid[i] = fork()) == 0){
			exit(100+i);
		}
	}
	
	for(i = 0; i < n; i++){
		pid_t wpid = waitpid(pid[i], &c_status, 0);
		printf("%d is waiting %d\n",getpid(), pid[i]);
		if(WIFEXITED(c_status)){
			printf("parent : %d, child : %d, status %d\n", getpid(), wpid, WEXITSTATUS(c_status));
		}
	}

	return 0;
}



		

