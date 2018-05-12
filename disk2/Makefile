TARGET = test
OBJECTS = testcase.o disk.o fs.o Matrix.o mount.o
SRCS = $(OBJECTS:.o=.c)

CC = gcc
CFLAGS = -g -c
.SUFFIXES : .c .o

$(TARGET) : $(OBJECTS) 
	$(CC) -g -pthread $(OBJECTS) -o $@ -lm
testcase.o : testcase.c fs.h
#	$(CC) $(CFLAGS) $*.c -o $@
disk.o : disk.c disk.h

fs.o : fs.c fs.h Matrix.h

Matrix.o : Matrix.c Matrix.h

mount.o : mount.c

clean : 
	rm $(OBJECTS)
