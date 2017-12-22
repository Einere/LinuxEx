test : main2.o MsgQueue.o Thread.o
	gcc -g -pthread main2.o MsgQueue.o Thread.o -o test
main2.o : main2.c Helper.h
	gcc -g -c main2.c -o main2.o
MsgQueue.o : MsgQueue.c Helper.h
	gcc -g -c MsgQueue.c -o MsgQueue.o
Thread.o : Thread.c Helper.h
	gcc -g -c Thread.c -o Thread.o
