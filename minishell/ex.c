#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

void hand(int signo){
	printf("hehe\n");
}


int main(){
	
	struct sigaction act;
	act.sa_handler = hand;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT, &act, NULL);

	while(1);


}

