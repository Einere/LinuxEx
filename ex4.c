#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
int n=0;

void handler(int signum){
	printf("signal %d recieved\n", signum);
	n=0;
}

int main(){
	signal(SIGINT, handler);

	while(1){
		printf("working %d\n",n++);
		sleep(1);
	}
}

