#ifndef TEST_CASE2_H
#define TEST_CASE2_H
 
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <memory.h>
#include <errno.h>
#include "Init.h"
#include "Thread.h"

#define TOTAL_THREAD_NUM		(5)

void* Tc2ThreadProc(void* parm);
void TestCase2(void);

#endif
