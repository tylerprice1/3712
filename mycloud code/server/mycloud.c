#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "mycloud.h"

void mycloud_printRequest (FILE * f, const struct mycloud_request * request) {
#if MYCLOUD_DEBUG
	unsigned int i;
	const char * ch;
#endif
	if (request != NULL && f != NULL) {
		/* type of request */
		fprintf(f, "REQUEST:\n");
		fprintf(f, "\ttype: ");
		if (request->type == STORE_REQUEST)
			fprintf(f, "store");
		else if (request->type == RETRIEVE_REQUEST)
			fprintf(f, "retrieve");
		else if (request->type == DELETE_REQUEST)
			fprintf(f, "delete");
		else if (request->type == LIST_REQUEST)
			fprintf(f, "list");
		else
			fprintf(f, "invalid");
		fprintf(f, "\n");
		/* secret key of request */
		fprintf(f, "\tkey: %u\n", request->key);
		/* file name */
		fprintf(f, "\tfile name: ");
		if (request->type == STORE_REQUEST || request->type == RETRIEVE_REQUEST || request->type == DELETE_REQUEST)
			fprintf(f, "\"%s\"", request->message.filename);
		else
			fprintf(f, "NONE");
		fprintf(f, "\n");
#if MYCLOUD_DEBUG
		/* file size */
		if (request->type == STORE_REQUEST) {
			fprintf(f, "\tfile size: %u bytes\n", request->message.size);
			/* file contents */
			if (request->message.size > 0) {
				fprintf(f, "\tfile contents as char array: \"");
				ch = (const char *)request->message.content;
				for (i = 0; i < request->message.size; i++) {
					fprintf(f, "%c", *ch++);
				}
				fprintf(f, "\"\n");
			}
		}
#endif
	}
}
void mycloud_printResponse (FILE * f, const struct mycloud_response * response) {
#if MYCLOUD_DEBUG
	unsigned int i;
	const char * ch;
#endif
	if (response != NULL && f != NULL) {
		/* status of response */
		/* type of response */
		fprintf(f, "RESPONSE:\n");
		fprintf(f, "\tstatus: %d\n", response->status);
		fprintf(f, "\tfile size: %u\n", response->message.size);
#if MYCLOUD_DEBUG
		if (response->message.size > 0) {
			fprintf(f, "\tfile contents as char array: \"");
			ch = (const char *)response->message.content;
			for (i = 0; i < response->message.size; i++) {
				fprintf(f, "%c", *ch++);
			}
			fprintf(f, "\"\n");
		}
#endif
	}
}



int mycloud_read (int fd, void * des, unsigned long bytes) {
	unsigned long bytes_read, bytes_left;
	char * pos;
	int err, eof;

#if MYCLOUD_DEBUG
	fprintf(stderr, "mycloud_read(fd = %d, des = %p, bytes = %lu)\n", fd, des, bytes);
#endif

	if (des != NULL) {
		bytes_left = bytes;
		bytes_read = 0;
		err = 0;
		eof = 0;
		errno = 0;
		pos = des;
		do {
			bytes_read = read(fd, pos, bytes_left);
#if MYCLOUD_DEBUG
			fprintf(stderr, "\t%d bytes read from %d\n", bytes_read, fd);
			fprintf(stderr, "\terrno = \"%s\"\n", strerror(errno));
#endif
			if (bytes_read < 0) {
				/* read error */
#if MYCLOUD_DEBUG
				fprintf(stderr, "\terrno = \"%s\"\n", strerror(errno));
#endif
				if (errno != EINTR) {
					err = 1;
				}
				/* else interrupted system call */
			}
			/* eof */
			else if (bytes_read == 0) {
#if MYCLOUD_DEBUG
				fprintf(stderr, "\teof reached for %d\n", fd);
#endif
				eof = 1;
			}
			/* successful read */
			else {
				bytes_left -= bytes_read;
				pos += bytes_read;
			}
		} while (!err && !eof && bytes_left > 0);
		bytes_read = bytes - bytes_left;
	}
	else {
		err = 1;
	}
#if MYCLOUD_DEBUG
	fprintf(stderr, "\treturning from mycloud_read\n");
#endif
	return (err) ? (-1) : (bytes_read);
}
int mycloud_write (int fd, const void * src, unsigned long bytes) {
	unsigned long bytes_written, bytes_left;
	const char * pos;
	int err = 0;

#if MYCLOUD_DEBUG
	fprintf(stderr, "mycloud_write(fd = %d, src = %p, bytes = %lu)\n", fd, src, bytes);
#endif

	if (src != NULL && fd >= 0) {
		bytes_written = 0;
		bytes_left = bytes;
		pos = src;
		do {
			bytes_written = write(fd, pos, bytes_left);
#if MYCLOUD_DEBUG
			fprintf(stderr, "\t%d bytes written to %d\n", bytes_written, fd);
#endif
			if (bytes_written < 0) {
				/* write error */
#if MYCLOUD_DEBUG
				fprintf(stderr, "\terrno = \"%s\"\n", strerror(errno));
#endif
				if (errno != EINTR) {
					err = 1;
				}
			}
			else if (bytes_written == 0) {
#if MYCLOUD_DEBUG
				fprintf(stderr, "\terrno = \"%s\"\n", strerror(errno));
#endif
			}
			else {
				bytes_left -= bytes_written;
				pos += bytes_written;
			}
		} while (!err && bytes_left > 0);
		bytes_written = bytes - bytes_left;
	}
	else {
		err = 1;
	}
#if MYCLOUD_DEBUG
	fprintf(stderr, "\treturning from mycloud_write\n");
#endif
	return (err) ? (-1) : (bytes_written);
}











