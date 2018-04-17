#ifndef MESSAGE_H
#define MESSAGE_H

#include <time.h>
#include "networking.h"

enum MessageType {
	/* errors */
	GENERAL_ERROR,
	INVALID_USERNAME,
	CHAT_FULL,
	/* others */
	NEW_MESSAGE,
	JOIN_REQUEST,
};

struct Message {
	char username[MAX_USERNAME];
	enum MessageType type;
	unsigned int length;
	char * text;
};
/**/
int  Message_init(struct Message * m, const char * username, const char * text);
void Message_destroy(struct Message * m);
/* SEND */
int  Message_ntoh(struct Message * m);
int  Message_read(struct Message * m, int fd);
/* RECEIVE */
int  Message_hton(struct Message * m);
int  Message_send(const struct Message * m, int fd);

struct Message * message_deepcopy(struct Message * destination, const struct Message * source);
/*
 * PREPARE
 * time_t -> gmtime
 * htonl
 * SEND
 * RECEIVE
 * PROCESS
 * gmtime -> time_t
 * time_t -> localtime
 */


#endif
