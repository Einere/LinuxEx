#ifndef TEST_CASE_1_H
#define TEST_CASE_1_H
 
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include "Thread.h"

#define TOTAL_THREAD_NUM (5)
void* Tc1ThreadProc(void* param);
void TestCase1(void);

#endif
