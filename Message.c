#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Message.h"
#include "networking.h"


#if 0 /* deprecated */
/* host/network conversions */
struct Message *
 Message_hton(struct Message * const m) {
	if (m != NULL) {
		m->type = htonl(m->type);
		m->length = htonl(m->length);
		return m;
	}
	return NULL;
}
struct Message *
 Message_ntoh(struct Message * const m) {
	if (m != NULL) {
		m->type = ntohl(m->type);
		m->length = ntohl(m->length);
		return m;
	}
	return NULL;
}
#endif
void Message_print(const struct Message * const m) {
	if (m != NULL) {
		printf("type: ");
		switch (m->type) {
			case INVALID_USERNAME:
				printf("INVALID_USERNAME");
				break;
			case CHAT_FULL:
				printf("CHAT_FULL");
				break;
			case GENERAL_ERROR:
				printf("GENERAL_ERROR");
				break;
			case SEND_ERROR:
				printf("SEND_ERROR");
				break;
			case NO_MESSAGE:
				printf("NO_MESSAGE");
				break;
			case NEW_MESSAGE:
				printf("NEW_MESSAGE");
				break;
			case JOIN_REQUEST:
				printf("JOIN_REQUEST");
				break;
			case EXIT_REQUEST:
				printf("EXIT_REQUEST");
				break;
			case LOCAL_MESSAGE:
				printf("LOCAL_MESSAGE");
				break;
			case SEND_SUCCESS:
				printf("SEND_SUCCESS");
				break;
			default:
				printf("INVALID MESSAGE TYPE");
				break;
		}
		printf("\n");
		printf("username: \"%s\"\n", m->username);
		if (m->length > 0) {
			printf("text: \"%s\"\n", m->text);
		}
		else {
			printf("text: \"%s\"\n", m->text);
		}
	}
}
/* read/write */
int
 Message_read(struct Message * const m, int fd) {
	if (m != NULL) {
		struct Message out; /* keep copy in case something failed along the way */
		int bytesRead, totalRead = 0;

		Message_init(&out);
		/* read type */
		bytesRead = robustRead(fd, &(out.type), sizeof(out.type));
		if (bytesRead == sizeof(out.type)) {
			totalRead += bytesRead;
			out.type = ntohl(out.type);
/*			fprintf(stderr, "received type = %d (enum value)\n", out.type); */
			/* read username */
			bytesRead = robustRead(fd, &(out.username), MAX_USERNAME);
			if (bytesRead == MAX_USERNAME) {
				totalRead += bytesRead;
/*				fprintf(stderr, "received username = %s\n", out.username); */
				/* read length of text (can be 0) */
				bytesRead = robustRead(fd, &(out.length), sizeof(out.length));
				if (bytesRead == sizeof(out.length)) {
					totalRead += bytesRead;
					out.length = ntohl(out.length);
/*					fprintf(stderr, "received length = %d\n", out.length); */
					if (out.length > 0) {
						/* read text */
						bytesRead = robustRead(fd, out.text, out.length);
/*						fprintf(stderr, "robustRead(text) returned %d\n", bytesRead); */
						if (bytesRead == out.length) {
							totalRead += bytesRead;
							out.text[out.length] = '\0';
/*							fprintf(stderr, "received text = %s\n", out.text); */
							Message_deepCopy(m, &out);
							return totalRead;
						}
					}
					else {
						Message_deepCopy(m, &out);
						return totalRead;
					}
				}
			}
		}
	}
	return -1;
}
int
 Message_write(const struct Message * const m, int fd) {
	if (m != NULL && fd >= 0) {
		int bytesWritten, totalWritten = 0;
		enum MessageType temp;
		/* type */
		temp = htonl(m->type);
		bytesWritten = robustWrite(fd, &(temp), sizeof(temp));
		if (bytesWritten == sizeof(temp)) {
			totalWritten += bytesWritten;
			/* username */
/*			fprintf(stderr, "sent type = %d (enum value)\n", m->type); */
			bytesWritten = robustWrite(fd, &(m->username), MAX_USERNAME);
			if (bytesWritten == MAX_USERNAME) {
				unsigned int temp;
				totalWritten += bytesWritten;
/*				fprintf(stderr, "sent username = %s\n", m->username); */
				/* length of text */
				temp = htonl(m->length);
				bytesWritten = robustWrite(fd, &temp, sizeof(temp));
				if (bytesWritten == sizeof(temp)) {
					totalWritten += bytesWritten;
/*					fprintf(stderr, "sent length = %d\n", m->length); */
					/* text */
					bytesWritten = robustWrite(fd, &(m->text), m->length);
					if (bytesWritten == m->length) {
						totalWritten += bytesWritten;
/*						fprintf(stderr, "sent text = %s\n", m->text); */
						return totalWritten;
					}
				}
			}
		}
	}
	return -1;
}
/* instance management */
struct Message *
 Message_init(struct Message * const message) {
	if (message != NULL) {
		memset(message, 0, sizeof(struct Message));
	}
	return message;
}
struct Message *
 Message_initAndSet(struct Message * const message,
                    const enum MessageType type,
                    const char * const username,
                    const char * const text)
{
	if (message != NULL && username != NULL && text != NULL) {
		register unsigned int len;
		len = strlen(text);

		message->type = type;
		strncpy(message->username, username, MAX_USERNAME);
		message->length = (len < MAX_MESSAGE) ? len : MAX_MESSAGE;
		strncpy(message->text, text, MAX_MESSAGE);

		return message;
	}
	return NULL;
}
void
 Message_destroy(struct Message * const m) {
	if (m != NULL) {
		Message_init(m);
	}
}
struct Message *
 Message_deepCopy(struct Message * destination, const struct Message * source) {
	if (destination != NULL && source != NULL) {
		memcpy(destination, source, sizeof(struct Message));
		return destination;
	}
	return NULL;
}
