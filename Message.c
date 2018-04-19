#include <string.h>
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
			/* read username */
			bytesRead = robustRead(fd, &(out.username), MAX_USERNAME);
			if (bytesRead == MAX_USERNAME) {
				totalRead += bytesRead;
				/* read length of text (can be 0) */
				bytesRead = robustRead(fd, &(out.length), sizeof(out.length));
				if (bytesRead == sizeof(out.length)) {
					totalRead += bytesRead;
					out.length = ntohl(out.length);
					/* allocate for text if needed */
					if (out.length > 0) {
						char * temp;
						temp = (char *)malloc(out.length * sizeof(char));
						if (temp != NULL) {
							out.text = temp;
							/* read text */
							bytesRead = robustRead(fd, out.text, out.length);
							if (bytesRead > 0) {
								totalRead += bytesRead;
								Message_deepCopy(m, &out);
								Message_destroy(&out);
								return totalRead;
							}
						}
					}
				}
			}
		}
	}
	return -1;
}
int
 Message_write(const struct Message * const m, int fd) {

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
		char * temp;
		unsigned int len;

		len = strlen(text);
		temp = (char *)malloc( sizeof(char) * (len + 1) ); /* +1 for '\0' */
		if (temp != NULL) {
			message->type = type;
			strncpy(message->username, username, MAX_USERNAME);
			message->length = len;
			message->text = temp;
			return message;
		}
	}
	return NULL;
}
void
 Message_destroy(struct Message * const m) {
	if (m != NULL) {
		if (m->text != NULL)
			free(m->text);
		Message_init(m);
	}
}
struct Message *
 Message_deepCopy(struct Message * destination, const struct Message * source) {
	if (destination != NULL && source != NULL) {
		char * temp;
		/* allocate text */
		if (destination->text != NULL) {
			temp = (char *)realloc(destination->text, source->length * sizeof(char));
		}
		else {
			temp = (char *)malloc(source->length * sizeof(char));
		}
		if (temp != NULL) {
			/* copy values */
			destination->type = source->type;
			strcpy(destination->username, source->username);
			destination->length = source->length;
			destination->text = temp;
			strcpy(destination->text, source->text);
			return destination;
		}
	}
	return NULL;
}
