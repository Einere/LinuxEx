TARGET = test
OBJECTS = testcase.o disk.o fs.o validate.o
SRCS = $(OBJECTS:.o=.c)

CC = gcc
CFLAGS = -g -c
.SUFFIXES : .c .o

$(TARGET) : $(OBJECTS) 
	$(CC) -g -pthread $(OBJECTS) -o $@
testcase.o : testcase.c validate.h
#	$(CC) $(CFLAGS) $*.c -o $@
disk.o : disk.c disk.h

fs.o : fs.c fs.h

clean : 
	rm $(OBJECTS)
