HEADERS=mycloud.h mycloudserver.h
OBJECTS=mycloud.o mycloudserver.o server.o
EXEC=mycloudserver

CC=gcc
#CFLAGS=-ansi -pedantic -Wall
CFLAGS=-Wall #-DMYCLOUD_DEBUG

$(EXEC): $(OBJECTS) $(HEADERS)
	$(CC) -o $(EXEC) $(OBJECTS)
clean:
	rm -f $(OBJECTS) $(EXEC)
