#include <ctype.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Message.h"
#include "Chat.h"
#include "networking.h"
#include "stringify.h"

#define MAX_TEXT 
#define MAX_ERROR 1024
#define IS_ERROR(err) ((err) < 0)

int usernameIsValid (const char * name);  /* returns bool */
int connectServer (const char * servername, const char * port, char * errorMessage);  /* returns socketfd, -1 on error */

struct Line {
	char * line;
	unsigned int capacity;
};

char *
readLine(struct Line * line);

/* label "errorstate" is located at the end of main */
int main(const int argc, const char * const * const argv) {
	int err = 0;
	char errorMessage[MAX_ERROR] = "";
	if (argc == 3) {
		struct Message toSend, received, * toPrint;
		Queue printQueue; /* Chat prints progress in this queue */
		pthread_mutex_t printMutex;
		struct Chat chat;
		int socketfd = -1;
		char username[MAX_USERNAME] = "";
		struct Line text;
		memset(&text, 0, sizeof(text));
		/* GET USERNAME */
		printf("Enter a username: ");
		err = IS_ERROR(scanf("%" stringify(MAX_USERNAME) "s", username));
		while ( !err && !usernameIsValid(username) ) {
			fprintf(stderr, "\"%s\" is not a valid username. ", username);
			fprintf(stderr, "Valid usernames contain fewer than %d characters, ", MAX_USERNAME);
			fprintf(stderr, "start with a letter or underscore, and continue with letters, numbers, or underscores\n");
			printf("Enter a username: ");
			err = IS_ERROR(scanf("%" stringify(MAX_USERNAME) "s", username));
		}
		/* err means stdin was probably closed (CTRL-D) */
		if (err) {
			strcpy(errorMessage, "stdin error");
			goto errorstate;
		}
		printf("Your username is \"%s\"\n", username);
		printf("Connecting to chat service...\n");
		/* OPEN CONNECTION WITH SERVER */
		socketfd = connectServer(argv[1], argv[2], errorMessage);
		err = IS_ERROR(socketfd);
		if (err) {
			goto errorstate;
		}
		/* START COMMUNICATING */
		printf("Connection established!\n");
		printf("Starting communication with server\n");
		/* open chat */
		Queue_initAndSet(&printQueue, sizeof(struct Message), NULL, NULL);
		pthread_mutex_init(&printMutex, NULL);
/*		Chat_init(&chat, socketfd, &printQueue, &printMutex); */
		Chat_init(&chat, socketfd, NULL,        NULL);

		printf("Starting chat. Enter '\\quit' to stop\n");
		printf("> ");
		while ( readLine(&text) != NULL && strcmp(text.line, "\\quit") != 0 ) {
			Message_initAndSet(&toSend, NEW_MESSAGE, username, text.line);
			/* send message */
			Chat_send(&chat, &toSend);
			/* receive messages */
			Chat_receive(&chat, &received);
			while (received.type == NEW_MESSAGE) {
				printf("%s: %s\n", received.username, received.text);
				Chat_receive(&chat, &received);
			}
			
			/* give updates from callback */
			pthread_mutex_lock(&printMutex);
			while ( !Queue_isEmpty(&printQueue) ) {
				 toPrint = (struct Message *) Queue_next(&printQueue);
				 printf("%s\n", toPrint->text);
				 Queue_dequeue(&printQueue);
			}
			pthread_mutex_unlock(&printMutex);

			printf("> ");
		}
		if (text.line != NULL) {
			fprintf(stderr, "freeing text.line = %p (%s)\n", (void *)text.line, text.line);
			free(text.line);
		}
		/* send exit request to server */
		Message_initAndSet(&toSend, EXIT_REQUEST, username, "<exiting chat>");
		Chat_send(&chat, &toSend);
		while (Chat_isMoreToSend(&chat)) {
			sleep(1);
		} /* wait until all messages are sent */

		/* receive any remaining messages */
		Chat_receive(&chat, &received);
		while (received.type == NEW_MESSAGE) {
			printf("%s: %s\n", received.username, received.text);
			Chat_receive(&chat, &received);
		}
		/* flush any remaining updates */
		pthread_mutex_lock(&printMutex);
		while ( !Queue_isEmpty(&printQueue) ) {
			 toPrint = (struct Message *) Queue_next(&printQueue);
			 printf("%s\n", toPrint->text);
			 Queue_dequeue(&printQueue);
		}
		pthread_mutex_unlock(&printMutex);
		/* close connection */
		close(socketfd);
	}
	else {
		sprintf(errorMessage, "Usage: %s <server name> <port>", argv[0]);
		goto errorstate;
	}
/*	system("ps"); */
	return 0;
errorstate:
/*	system("ps"); */
	fprintf(stderr, "ERROR: \"%s\"\n", errorMessage);
	return -1;
}

char *
readLine(struct Line * const line) {
	if (line != NULL) {
		if (line->line == NULL) {
			line->capacity = 1024; /* an initial size */
			line->line = (char *)calloc(line->capacity, sizeof(char));
		}
		if (line->line != NULL) {
			unsigned int i = 0;
			char ch;
			ch = getchar();
			while (isspace(ch))
				ch = getchar();
			while (ch != '\n' && ch != EOF) {
				if (i == line->capacity) {
					char * temp;
					unsigned int newCapacity;
					newCapacity = line->capacity << 1;
					temp = (char *)realloc(line->line, newCapacity * sizeof(char));
					if (temp != NULL) {
						fprintf(stderr, "readLine(): capacity increased to %u\n", newCapacity);
						line->line = temp;
						line->capacity = newCapacity;
					}
					else {
						/* can't allocate anymore */
						fprintf(stderr, "readLine(): alloc error\n");
						return NULL;
					}
				}

				line->line[i++] = ch;
				ch = getchar();
			}
			line->line[i] = '\0';
			return line->line;
		}
	}
	return NULL;
}

int usernameIsValid (const char * const name) {
	const char * pos = name;
	if ( isalpha(*pos) || *pos == '_' ) {
		pos++;
		while (*pos != '\0') {
			if ( !isalnum(*pos) && *pos != '_' && *pos != '\0') {
				return 0;
			}
			pos++;
		}
		return 1;
	}
	return 0;
}
int connectServer (const char * const servername, const char * const port, char * const errorMessage) {
	/* VALIDATE ARGUMENTS */
	if (servername == NULL || port == NULL) {
		if (errorMessage != NULL) {
			strcpy(errorMessage, "connectServer: Invalid argument(s)");
		}
		/* goto errorstate */
	}
	else {
		int err;
		int socketfd;
		struct addrinfo hints;
		struct addrinfo * linkedlist = NULL;
		struct addrinfo * node = NULL;
		/* hints */
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET; /* IPv4 */
		hints.ai_socktype = AI_PASSIVE; /* getaddrinfo will fill in IP */
		hints.ai_socktype = SOCK_STREAM; /* connection type */
		/* getaddrinfo */
		err = IS_ERROR(getaddrinfo(servername, port, &hints, &linkedlist));
		if (err || linkedlist == NULL) {
			if (errorMessage != NULL) {
				strcpy(errorMessage, "connectServer: Error getting address info");
			}
			goto errorstate;
		}
		/* socket */
		node = linkedlist;
		socketfd = socket(PF_INET, SOCK_STREAM, 0);
		err = IS_ERROR(socketfd);
		if (err) {
			freeaddrinfo(linkedlist);
			if (errorMessage != NULL) {
				strcpy(errorMessage, "connectServer: Error getting socket");
			}
			goto errorstate;
		}
		/* connect */
		do {
			err = IS_ERROR(connect(socketfd, (struct sockaddr *)(node->ai_addr), node->ai_addrlen));
			node = node->ai_next;
		} while (err && node != NULL);
		if (err) {
			freeaddrinfo(linkedlist);
			close(socketfd);
			if (errorMessage != NULL) {
				strcpy(errorMessage, "connectServer: Error creating connection");
			}
			goto errorstate;
		}
		/* done */
		return socketfd;
	}
errorstate:
	return -1;
}



















