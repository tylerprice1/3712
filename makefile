QUEUE_SOURCES =               Queue/LinkedList.c Queue/LinkedListNode.c
QUEUE_HEADERS = Queue/Queue.h Queue/LinkedList.h Queue/LinkedListNode.h

CLIENT_EXEC = client
CLIENT_SOURCES = client.c message.c chat.c $(QUEUE_SOURCES)
CLIENT_HEADERS =          message.h chat.h $(QUEUE_HEADERS)

SERVER_EXEC = server
SERVER_SOURCES = server.c message.c chat.c $(QUEUE_SOURCES)
SERVER_HEADERS =          message.h chat.h $(QUEUE_HEADERS)

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
