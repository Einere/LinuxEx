example : Thread.o TestMain.o Scheduler.o TestCase1.o TestCase2.o TestCase3.o Init.o
	gcc -g -pthread Thread.o TestMain.o Scheduler.o TestCase1.o TestCase2.o TestCase3.o Init.o -o example
Thread.o : Thread.c Helper.h
	gcc -g -pthread -c Thread.c -o Thread.o
TestMain.o : TestMain.c
	gcc -g -pthread -c TestMain.c -o TestMain.o
Scheduler.o : Scheduler.c Helper.h
	gcc -g -pthread -c Scheduler.c -o Scheduler.o
TestCase1.o : TestCase1.c
	gcc -g -pthread -c TestCase1.c -o TestCase1.o
TestCase2.o : TestCase2.c
	gcc -g -pthread -c TestCase2.c -o TestCase2.o
TestCase3.o : TestCase3.c
	gcc -g -pthread -c TestCase3.c -o TestCase3.o
Init.o : Init.c Helper.h
	gcc -g -pthread -c Init.c -o Init.o
