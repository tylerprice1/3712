#ifndef MESSAGE_H
#define MESSAGE_H

#include <time.h>
#include "networking.h"

#define MESSAGE_CAPACITY (1024)
#define MAX_MESSAGE (MESSAGE_CAPACITY - 1)

enum MessageType {
	/* errors */
	INVALID_USERNAME = -4,
	CHAT_FULL,
	GENERAL_ERROR,
	SEND_ERROR,
	/* others */
	NO_MESSAGE = 0, /* empty message: set by Message_init */
	NEW_MESSAGE,
	JOIN_REQUEST,
	EXIT_REQUEST,
	LOCAL_MESSAGE, /* using for communication within client/server (e.g., printQueue) */
	SEND_SUCCESS
};

struct Message {
	enum MessageType type;
	char username[MAX_USERNAME];
	unsigned int length;
	char text[MESSAGE_CAPACITY];
};
/* instance management */
struct Message *  Message_init(struct Message * m);
struct Message *  Message_initAndSet(struct Message * m, enum MessageType type, const char * username, const char * text);
void Message_destroy(struct Message * m);
struct Message *  Message_deepCopy(struct Message * destination, const struct Message * source);
#if 0 /* deprecated */
/* HOST/NETWORK CONVERSIONS */
struct Message *  Message_hton(struct Message * m);
struct Message *  Message_ntoh(struct Message * m);
#endif
void Message_print(const struct Message * m);
/* READ/WRITE */
int  Message_write(const struct Message * m, int fd);
int  Message_read(struct Message * m, int fd);
#endif
