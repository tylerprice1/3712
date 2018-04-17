HEADERS=mycloud.h mycloudclient.h
OBJECTS=mycloud.o mycloudclient.o client.o
EXEC=mycloudclient
CC=gcc
#CFLAGS=-ansi -pedantic -Wall -DMYCLOUD_DEBUG
CFLAGS=-g -Wall #-DMYCLOUD_DEBUG

$(EXEC): $(OBJECTS) $(HEADERS)
	$(CC) -o $(EXEC) $(OBJECTS)
clean:
	rm -f $(OBJECTS) $(EXEC)
