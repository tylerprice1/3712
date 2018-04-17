CLIENT_EXEC = client
CLIENT_SOURCES = client.c message.c                      Queue/LinkedList.c Queue/LinkedListNode.c
CLIENT_HEADERS =          message.h chat.h Queue/Queue.h Queue/LinkedList.h Queue/LinkedListNode.h

SERVER_EXEC = server
SERVER_SOURCES = server.c message.c                      Queue/LinkedList.c Queue/LinkedListNode.c
SERVER_HEADERS =          message.h chat.h Queue/Queue.h Queue/LinkedList.h Queue/LinkedListNode.h

CC = gcc
CFLAGS = -ansi -pedantic -Wall -Werror

all: $(CLIENT_HEADERS) $(SERVER_HEADERS) $(CLIENT_SOURCES) $(SERVER_SOURCES)
	$(CC) $(CFLAGS) -o $(SERVER_EXEC) $(SERVER_SOURCES)
	$(CC) $(CFLAGS) -o $(CLIENT_EXEC) $(CLIENT_SOURCES)
$(CLIENT_EXEC): $(CLIENT_HEADERS) $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) -o $(CLIENT_EXEC) $(CLIENT_SOURCES)
$(SERVER_EXEC): $(SERVER_HEADERS) $(SERVER_SOURCES)
	$(CC) $(CFLAGS) -o $(SERVER_EXEC) $(SERVER_SOURCES)
clean:
	rm -f $(CLIENT_EXEC) $(SERVER_EXEC) *.o
