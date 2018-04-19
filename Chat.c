
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h> /* sleep */
#include "Chat.h"
#include "networking.h"
#include "Message.h"


struct Chat *  Chat_init(struct Chat * chat, int fd, Queue * printQueue, pthread_mutex_t * printMutex) {
	if (chat != NULL && fd >= 0) {
		/* SOCKET */
		chat->fd = fd;
		/* QUEUES */
		chat->printQueue = printQueue;
		chat->printMutex = printMutex;
		Queue_initAndSet( &(chat->send.queue),    sizeof(struct Message), NULL, NULL );
		Queue_initAndSet( &(chat->receive.queue), sizeof(struct Message), NULL, NULL );
		/* THREADS */
		pthread_mutex_init( &(chat->send.mutex),    NULL );
		pthread_mutex_init( &(chat->receive.mutex), NULL );
		pthread_create( &(chat->send.thread),    NULL, &(Chat_sendCallback),    (void *)(chat) );
		pthread_create( &(chat->receive.thread), NULL, &(Chat_receiveCallback), (void *)(chat) );
		return chat;
	}
	return NULL;
}

void
 Chat_send(struct Chat * const chat, const struct Message * const message) {
	if (chat != NULL && message != NULL) {
		/* add to send queue */
		pthread_mutex_lock( &(chat->send.mutex) );  /* LOCK */
		Queue_enqueue( &(chat->send.queue), message);
		pthread_mutex_unlock( &(chat->send.mutex) );  /* UNLOCK */
	}
}
void
 Chat_receive(struct Chat * const chat, struct Message * const message) {
	if (chat != NULL && message != NULL) {
		if ( !Queue_isEmpty( &chat->receive.queue ) ) {
			const struct Message * const NEXT = Queue_next( &chat->receive.queue );
			/* copy */
			Message_deepCopy(message, NEXT);
			/* remove from receive queue */
			pthread_mutex_lock( &(chat->receive.mutex) );  /* LOCK */
			Queue_dequeue( &(chat->receive.queue) );
			pthread_mutex_unlock( &(chat->receive.mutex) );  /* UNLOCK */
		}
	}
}

void *  Chat_sendCallback(void * void_chat) {
	struct Chat * const chat = void_chat;
	if (chat != NULL) {
		char report[1024] = "";
		char substr[16] = "";
		struct Message message;
		int err = 0;
		/* while socket is still open */
		while ( !err ) {
			/* a pending message */ 
			if ( ! Queue_isEmpty( &(chat->send.queue) ) ) {
				/* get message */
				message = *(struct Message *)Queue_next( &chat->send.queue );
				/* remove from queue */
				pthread_mutex_lock( &(chat->send.mutex) );  /* LOCK */
				Queue_dequeue(&chat->send.queue);
				pthread_mutex_unlock( &(chat->send.mutex) );  /* UNLOCK */
				/* send */
				err = IS_ERROR(Message_write(&message, chat->fd));
				if (chat->printQueue != NULL) {
					/* report to main thread */
					strncpy(substr, message.text, sizeof(substr));
					if (IS_ERROR(err)) {
						sprintf(report, "Failed to send message starting with \"%s\"", substr);
					}
					else {
						sprintf(report, "Sent message starting with \"%s\"!", substr);
					}
					pthread_mutex_lock( chat->printMutex );  /* LOCK */
					Queue_enqueue( chat->printQueue, substr);
					pthread_mutex_unlock( chat->printMutex );  /* UNLOCK */
				}
			}
			/* no message: wait a small amount of time and try again */
			else {
				sleep(CHAT_WAIT_TIME);
			}
		}
	}
	return NULL;
}

void *  Chat_receiveCallback(void * void_chat) {
	fprintf(stderr, "Chat_receiveCallback() isn't done yet :(\n");
	return NULL;
}























