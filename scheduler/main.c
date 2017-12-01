#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"

#include <stdio.h>
#include <string.h>
int main(void)
{
	Thread a;
	memset(&a, 0, sizeof(Thread));
	a.tid = 1;
	a.pPrev = NULL;
	a.pNext = NULL;
	//rq_push(&a);
	wq_push(&a);

	Thread b;
	memset(&b, 0, sizeof(Thread));
	b.tid = 2;
	b.pPrev = NULL;
	b.pNext = NULL;
	//rq_push(&b);
	wq_push(&b);

	Thread c;
	memset(&c, 0, sizeof(Thread));
	c.tid = 3;
	c.pPrev = NULL;
	c.pNext = NULL;
	//rq_push(&c);
	wq_push(&c);

	Thread d;
	memset(&d, 0, sizeof(Thread));
	d.tid = 4;
	d.pPrev = NULL;
	d.pNext = NULL;
	//rq_push(&d);
	wq_push(&d);

	/*
	print_rq();
	rq_pop();
	print_rq();
	rq_remove(3);
	print_rq();
	*/
	print_wq();
	wq_pop();
	print_wq();
	wq_remove(3);
	print_wq();
	

	return 0;
}

