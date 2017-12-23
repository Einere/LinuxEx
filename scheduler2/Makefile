TARGET = test
OBJECTS = Init.o MsgQueue.o Scheduler.o Thread.o TestMain.o TestCase1.o TestCase2.o TestCase3.o TestCase4.o 
SRCS = $(OBJECTS:.o=.c)

CC = gcc
CFLAGS = -g -c
.SUFFIXES : .c .o

$(TARGET) : $(OBJECTS) 
	$(CC) -g -pthread $(OBJECTS) -o $@
Init.o : Init.c Helper.h
#	$(CC) $(CFLAGS) $*.c -o $@
MsgQueue.o : MsgQueue.c Helper.h
#	$(CC) $(CFLAGS) $*.c -o $@
Scheduler.o : Scheduler.c Init.h Thread.h Helper.h
#	$(CC) $(CFLAGS) $*.c -o $@
Thread.o : Thread.c Scheduler.h Helper.h
#	$(CC) $(CFLAGS) $*.c -o $@
TestMain.o : TestMain.c
#	$(CC) $(CFLAGS) $*.c -o $@
TestCase1.o : TestCase1.c
#	$(CC) $(CFLAGS) $*.c -o $@
TestCase2.o : TestCase2.c
#	$(CC) $(CFLAGS) $*.c -o $@
TestCase3.o : TestCase3.c
#	$(CC) $(CFLAGS) $*.c -o $@
TestCase4.o : TestCase4.c
#	$(CC) $(CFLAGS) $*.c -o $@
clean : 
	rm $(OBJECTS)
