TARGET = test
OBJECTS = my_main.o disk.o fs.o
SRCS = $(OBJECTS:.o=.c)

CC = gcc
CFLAGS = -g -c
.SUFFIXES : .c .o

$(TARGET) : $(OBJECTS) 
	$(CC) -g -pthread $(OBJECTS) -o $@
my_main.o : my_main.c
#	$(CC) $(CFLAGS) $*.c -o $@
disk.o : disk.c disk.h

fs.o : fs.c fs.h

clean : 
	rm $(OBJECTS)
