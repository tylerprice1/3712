
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h> /* sleep */
#include "Chat.h"
#include "networking.h"
#include "Message.h"


struct Chat *  Chat_init(struct Chat * const chat, int fd, Queue * const printQueue, pthread_mutex_t * const printMutex) {
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

/* basically de-init function */
void Chat_close(struct Chat * const chat) {
	pthread_mutex_destroy ( &chat->send.mutex );
	pthread_mutex_destroy ( &chat->receive.mutex );
	pthread_kill( chat->send.thread, SIGKILL );
	pthread_kill( chat->receive.thread, SIGKILL );
	Queue_destroy( &chat->send.queue );
	Queue_destroy( &chat->receive.queue );
	memset(chat, 0, sizeof(struct Chat));
}

int  Chat_isMoreToSend(const struct Chat * const chat) {
	return !Queue_isEmpty(&(chat->send.queue));
}

void
 Chat_send(struct Chat * const chat, const struct Message * const message) {
	if (chat != NULL && message != NULL) {
		/* add to send queue */
/*		fprintf(stderr, "adding to send queue: %s\n", message->text); */
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
#if 0
			printf("Chat_receive new message:\n");
			Message_print(message);
			printf("\n");
#endif
			/* remove from receive queue */
			pthread_mutex_lock( &(chat->receive.mutex) );  /* LOCK */
			Queue_dequeue( &(chat->receive.queue) );
			pthread_mutex_unlock( &(chat->receive.mutex) );  /* UNLOCK */
		}
		else {
			message->type = NO_MESSAGE;
		}
	}
}

void *  Chat_sendCallback(void * void_chat) {
	struct Chat * const chat = void_chat;
	if (chat != NULL) {
		char substr[16] = "";
		struct Message message, report;
		int err = 0;
		int bytesWritten;
		report.type = LOCAL_MESSAGE;
/*		fprintf(stderr, "sendCallback\n"); */
		/* while socket is still open */
		while ( !err ) {
/*			fprintf(stderr, "sendCallback loop\n"); */
			/* a pending message */
			if ( ! Queue_isEmpty( &(chat->send.queue) ) ) {
				/* get message */
				message = *(struct Message *)Queue_next( &chat->send.queue );
				/* send */
#if 0
				printf("sender sending message:\n");
				Message_print(&message);
				printf("\n");
#endif
/*				fprintf(stderr, "sending message: %s\n", message.text); */
				bytesWritten = Message_write(&message, chat->fd);
				err = IS_ERROR(bytesWritten) || bytesWritten == 0;
				/* remove from queue */
				if (!err) {
					pthread_mutex_lock( &(chat->send.mutex) );  /* LOCK */
					Queue_dequeue(&chat->send.queue);
					pthread_mutex_unlock( &(chat->send.mutex) );  /* UNLOCK */
				}
#if 0
				printf("sender sent message:\n");
				Message_print(&message);
#endif
				/* report */
				if (chat->printQueue != NULL && chat->printMutex != NULL) {
/*					fprintf(stderr, "sent message: %s\n", message.text); */
					/* report to main thread */
					strncpy(substr, message.text, sizeof(substr));
					if (err) {
						sprintf(report.text, "Failed to send message starting with \"%s...\"", substr);
					}
					else {
						sprintf(report.text, "Sent message starting with \"%s\"!", substr);
					}
					pthread_mutex_lock( chat->printMutex );  /* LOCK */
					Queue_enqueue( chat->printQueue, &report );
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
	struct Chat * const chat = void_chat;
/*	fprintf(stderr, "Chat_receiveCallback() isn't done yet :(\n"); */

	if (chat != NULL) {
		struct Message message;
		int err = 0;
/*		fprintf(stderr, "receiveCallback\n"); */
		while ( !err ) {
/*			fprintf(stderr, "receiveCallback loop\n"); */
			err = IS_ERROR(Message_read(&message, chat->fd));
#if 0
			printf("%d ? receive error in callback for user with username \"%s\"\n", err, chat->username);
			printf("Message received:\n");
			Message_print(&message);
			printf("\n");
#endif
			if (!err) {
/*				fprintf(stderr, "received message: %s\n", message.text); */
				/* add to queue */
				pthread_mutex_lock( &(chat->receive.mutex) );  /* LOCK */
				Queue_enqueue(&chat->receive.queue, &message);
				pthread_mutex_unlock( &(chat->receive.mutex) );  /* UNLOCK */
			}
		}
	}
	return NULL;
}























