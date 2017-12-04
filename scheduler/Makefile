example : Thread.o main.o Scheduler.o
	gcc -g -pthread Thread.o main.o Scheduler.o -o example
Thread.o : Thread.c
	gcc -g -pthread -c Thread.c -o Thread.o
main.o : main.c
	gcc -g -pthread -c main.c -o main.o
Scheduler.o : Scheduler.c
	gcc -g -pthread -c Scheduler.c -o Scheduler.o
