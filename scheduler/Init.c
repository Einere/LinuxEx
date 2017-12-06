#include "Init.h"
#include "Helper.h"

void Init(void){
	pthread_mutex_init(&static_mutex, NULL);
	pthread_cond_init(&static_cond, NULL);
	is_pushed = false;	
	RunQHead = NULL;
	JoinQHead = NULL;
	JoinQTail = NULL;
}

