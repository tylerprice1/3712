#ifndef CHAT_H
#define CHAT_H

#define CHAT_WAIT_TIME 2u

#include "Message.h"
#include "networking.h"
#include "Queue/Queue.h"

struct Chat {
	char username[MAX_USERNAME];
	int fd;
	Queue * printQueue;
	pthread_mutex_t * printMutex;
	struct {
		Queue queue;
		pthread_mutex_t mutex;
		pthread_t thread;
	} send, receive;
};

struct Chat *  Chat_init(struct Chat * chat, int fd, Queue * printQueue, pthread_mutex_t * printMutex);
int  Chat_close(struct Chat * chat);

void  Chat_send(struct Chat * chat, const struct Message * message);
void  Chat_receive(struct Chat * chat, struct Message * message);

int  Chat_isMoreToSend(struct Chat * chat);

/*
 * these ALWAYS return NULL
 * the (void *) return type was
 * required by pthread
 */
void *  Chat_sendCallback(void * chat);
void *  Chat_receiveCallback(void * chat);

#endif
