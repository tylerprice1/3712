
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
		socklen_t clientLength;
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
		if ( IS_ERROR(listen(listenfd, 1024)) ) {
			fprintf(stderr, "Listen error\n");
			return -1;
		}
		pthread_mutex_init(&forwarding.mutex, NULL);
		pthread_create(&forwarding.thread, NULL, &messageForwarder, NULL);

		/* listen "thread" */
		while (1) {
			clientLength = sizeof(clientAddress);
			connectionfd = accept( listenfd, (struct sockaddr *)&clientAddress, &clientLength );
			if (connectionfd >= 0) {
				SCT * const client = LinkedList_pushBack(&clients, &newConnection);
				Chat_init( &client->chat, connectionfd, NULL, NULL );
				Chat_receive( &client->chat, &message);
				while (message.type == NO_MESSAGE) {
					sleep(1);
					Chat_receive( &client->chat, &message);
				}
#if 0
				printf("message received:\n");
				Message_print(&message);
				printf("\n");
#endif

				if (message.type == JOIN_REQUEST) {
#if 1
					printf("client joined with username: \"%s\"\n", message.username);
#endif
					strcpy(client->chat.username, message.username);
					pthread_create(&client->thread, NULL, &clientHandler, client);
					pthread_mutex_init(&client->mutex, NULL);

					Message_initAndSet( &message, NEW_MESSAGE, "<server>", "hello from the server");
					Chat_send(&client->chat, &message);
				}
				else {
#if 0
					fprintf(stderr, "client joined incorrectly... closing connection\n");
#endif
					LinkedList_popBack(&clients);
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
		if ( !Queue_isEmpty(&forwarding.queue) ) {
			message = (struct Message *)Queue_next(&forwarding.queue);
#if 0
			printf("forwarding message to clients:\n");
			Message_print(message);
			printf("\n");
#endif

			/* forward message to clients */
			curr = clients.head;
			while (curr != NULL) {
				client = curr->data;
				/* don't send message to client who sent it */
				if ( strcmp(client->chat.username, message->username) != 0 ) {
					pthread_mutex_lock( &client->mutex );
					Chat_send( &client->chat, message);
					pthread_mutex_unlock( &client->mutex );
				}
				curr = curr->next;
			}
			/* remove message from queue */
			pthread_mutex_lock( &forwarding.mutex );
			Queue_dequeue(&forwarding.queue);
			pthread_mutex_unlock( &forwarding.mutex );
		}
		else {
			sleep(1);
		}
	}
#if 0
	printf("exiting messageForwarder\n");
#endif
	return NULL;
}
/* handlers messages from clients */
void *
clientHandler(void * void_sct) {
	SCT * client = (SCT *)void_sct;
	struct Message m;

#if 0
	printf("starting clientHandler for client with username \"%s\"\n", client->chat.username);
#endif
	while (1) {
		/* --- receive messages (add to forward queue) --- */
		/* get message */
		pthread_mutex_lock( &(client->mutex) );
		Chat_receive( &(client->chat), &m );
		pthread_mutex_unlock( &(client->mutex) );
		while (m.type == NO_MESSAGE) {
			sleep(1);
			pthread_mutex_lock( &(client->mutex) );
			Chat_receive( &(client->chat), &m );
			pthread_mutex_unlock( &(client->mutex) );
		}
#if 1
		printf("user \"%s\" sent message:\n", client->chat.username);
		Message_print(&m);
		printf("\n");
#endif
		/* check type */
		if (m.type == NEW_MESSAGE) {
			/* add to forward queue */
			pthread_mutex_lock( &forwarding.mutex );
			Queue_enqueue( &forwarding.queue, &m );
			pthread_mutex_unlock( &forwarding.mutex );
		}
		else if (m.type == EXIT_REQUEST) {
			/* remove client (arg) from global list */
#if 0
			printf("removing client: %p\n", client);
#endif
			pthread_mutex_lock( &(client->mutex) );
			LinkedList_remove( &clients, client );
			pthread_mutex_unlock( &(client->mutex) );
			return NULL;
		}
#if 0
		/* --- send messages (from forward queue) --- */
		if ( !Queue_isEmpty( &forwarding.queue ) ) {

		}
#endif
	}
#if 0
	printf("exiting clientHandler\n");
#endif
	return NULL;
}




























