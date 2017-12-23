#ifndef TEST_CASE_1_H
#define TEST_CASE_1_H
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sys/msg.h>

#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"
#include "MsgQueue.h"

void TestCase1(void);
#endif
