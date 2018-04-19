#include <ctype.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "Message.h"
#include "Chat.h"
#include "networking.h"
#include "stringify.h"

#define MAX_ERROR 1024
#define IS_ERROR(err) ((err) < 0)

int usernameIsValid (const char * name);  /* returns bool */
int connectServer (const char * servername, const char * port, char * errorMessage);  /* returns socketfd, -1 on error */

/* label "errorstate" is located at the end of main */
int main(const int argc, const char * const * const argv) {
	int err = 0;
	char errorMessage[MAX_ERROR] = "";
	if (argc == 3) {
		int socketfd = -1;
		char username[MAX_USERNAME] = "";
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
			/* strcpy(errorMessage, "Could not establish connection with server"); */
			goto errorstate;
		}
		/* START COMMUNICATING */
		printf("Connection established");
		printf("Work in progress. Exiting...\n");

		close(socketfd);
	}
	else {
		sprintf(errorMessage, "Usage: %s <server name> <port>", argv[0]);
		goto errorstate;
	}
	return 0;
errorstate:
	fprintf(stderr, "ERROR: \"%s\"\n", errorMessage);
	return -1;
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



















