
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "Chat.h"

#define IS_ERROR(err) ((err) < 0)

/* Message Forwarding Thread */
struct MessageForwardingThread {
	Queue queue;
	pthread_mutex_t mutex;
	pthread_t thread;
};
typedef  struct MessageForwardingThread  MFT;
/* Server Connection Threads */
struct ServerConnectionThread {
	struct Chat chat;
	unsigned int size;
	pthread_t thread;
	pthread_mutex_t mutex;
};
typedef  struct ServerConnectionThread  SCT;
/* handler functions for pthread */
void *
clientHandler(void * void_sct);
void *
messageForwarder(void * void_mft);


/* ----- MAIN ----- */
static LinkedList clients;
struct MessageForwardingThread forwarding;

int main(int argc, char ** argv) {
	if (argc == 2) {
		int clientLength;
		int listenfd;
		int connectionfd, port;
		struct Message message;
		struct sockaddr_in clientAddress, serverAddress;
		struct ServerConnectionThread newConnection;

		LinkedList_initAndSet(&clients, sizeof(SCT), NULL, NULL);
		Queue_initAndSet(&forwarding.queue, sizeof(struct Message), NULL, NULL);

		port = atoi(argv[1]);
		/* GET LISTEN FILE DESCRIPTOR */
		listenfd = socket(AF_INET, SOCK_STREAM, 0);
		if (listenfd < 0) {
			fprintf(stderr, "Error with socket()\n");
			return -1;
		}
		memset(&serverAddress, 0, sizeof(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons((unsigned short)port);
		if ( IS_ERROR(bind(listenfd, (struct sockaddr *)(&serverAddress), sizeof(serverAddress))) ) {
			fprintf(stderr, "Bind error\n");
			return -1;
		}
		if ( IS_ERROR(listen(listenfd, LISTENQ)) ) {
			fprintf(stderr, "Listen error\n");
			return -1;
		}

		/* listen "thread" */
		while (1) {
			clientLength = sizeof(clientAddress);
			connectionfd = accept( listenfd, &clientAddress, &clientLength );
			if (connectionfd >= 0) {
				Chat_initAndSet( &(newConnection.chat), connectionfd, NULL, NULL );
				Chat_receive(&(newConnection.chat), &message);
				if (message.type == JOIN_REQUEST) {
					strcpy(newConnection.chat.username, message.username);
					SCT * const client = LinkedList_pushBack(&clients, &newConnection);
					pthread_create(&client->thread, NULL, &clientHandler, client);
					pthread_mutex_init(&client->mutex, NULL);
				}
				else {
					Chat_close(&newConnection.chat);
					close(connectionfd);
				}
			}
		}
	}
	else {
		fprintf(stderr, "Usage, %s <port>\n", argv[0]);
	}
}

/* handles message forwarding */
void *
messageForwarder(void * void_mft) {
	struct Message * message;
	struct LinkedListNode * curr;
	SCT * client;

	while (1) {
		if ( !Queue_isEmpty( &forwarding.queue ) ) {
			message = (struct Message *)Queue_next(&forwarding.queue);
			/* forward message to clients */
			curr = clients.head;
			while (curr != NULL) {
				client = curr->data;
				/* don't send message to user who sent it */
				if ( strcmp(client->chat.username, message->username) != 0 ) {
					pthread_mutex_lock( &client->mutex );
					Chat_send( &client->chat, &m);
					pthread_mutex_unlock( &client->mutex );
				}
				curr = curr->next;
			}
		}
	}
}
/* handlers messages from clients */
void *
clientHandler(void * void_sct) {
	SCT * client = (SCT *)void_sct;
	struct Message m;

	while (1) {
		/* --- receive messages (add to forward queue) --- */
		/* get message */
		pthread_mutex_lock( &(client->mutex) );
		Chat_receive( &(client->chat), &m );
		pthread_mutex_unlock( &(client->mutex) );
		/* check type */
		if (m.type == NEW_MESSAGE) {
			/* add to forward queue */
			pthread_mutex_lock( &forwarding.mutex );
			Queue_enqueue( &forwarding.queue, &m );
			pthread_mutex_unlock( &forwarding.mutex );
		}
		else if (m.type == EXIT_REQUEST) {
			/* remove client (arg) from global list */
			LinkedList_remove( &clients, client );
			return NULL;
		}
#if 0
		/* --- send messages (from forward queue) --- */
		if ( !Queue_isEmpty( &forwarding.queue ) ) {

		}
#endif
	}
	return NULL;
}




























