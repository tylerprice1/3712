#ifndef mycloud_h
#define mycloud_h

#define MAX_FILENAME 80
#define MAX_FILESIZE 102400 /* 100 KiB */
#define MAX_STRING   1024
#define LOCAL_HOSTP  "127.0.0.1" /* #define LOCAL_HOSTN  (0x7fff 0000 0000 0001 u) */

#define    STORE_REQUEST 1u
#define RETRIEVE_REQUEST 2u
#define   DELETE_REQUEST 3u
#define     LIST_REQUEST 4u

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>

struct mycloud_request {
	unsigned int type;
	unsigned int key;
	struct {
		char filename[MAX_FILENAME];
		unsigned int size;
		void * content;
	} message;
};
struct mycloud_response {
	int status;
	struct {
		unsigned int size;
		void * content;
	} message;
};

/* connection.destination.server.address.{...} */
struct mycloud_connection {
	struct sockaddr_in address;
	int socketfd;
	unsigned int key;
#if 1
	union {
		struct {
			struct sockaddr_storage storage;
		} client;
		struct {
			char hostname[MAX_STRING];
		} server;

	} destination;
#else
	struct {
		char hostname[MAX_STRING];
		struct sockaddr_in address;
	} server;
#endif
};

void mycloud_printRequest  (FILE *, const struct mycloud_request *);
void mycloud_printResponse (FILE *, const struct mycloud_response *);

int  mycloud_read   (int fd,       void * des, unsigned long bytes);
int  mycloud_write  (int fd, const void * src, unsigned long bytes);

#endif
