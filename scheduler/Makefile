example : Thread.o main.o Scheduler.o 
	gcc -pthread Thread.o main.o Scheduler.o -o example
Thread.o : Thread.c
	gcc -pthread -c Thread.c -o Thread.o
main.o : main.c
	gcc -pthread -c main.c -o main.o
Scheduler.o : Scheduler.c
	gcc -pthread -c Scheduler.c -o Scheduler.o
