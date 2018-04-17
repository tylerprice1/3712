#ifndef mycloudclient_h
#define mycloudclient_h

#include <sys/types.h>
#include <sys/socket.h>
#include "mycloud.h"

#if 0 /* REFERENCES */
int socket(int domain, int type, int protocol);
socket(AF_INET, SOCK_STREAM, 0);
int connect(int sockfd, struct sockaddr * serv_addr, int addrlen);

struct sockaddr {
	unsigned short  sa_family;    // address family, AF_xxx
	char            sa_data[14];  // 14 bytes of protocol address
};

struct sockaddr_in {
	short int          sin_family;  /* Address family, AF_INET */
	unsigned short int sin_port;    /* Port number */
	struct in_addr     sin_addr;    /* Internet address */
	unsigned char      sin_zero[8]; /* Same size as struct sockaddr */
};
#endif

int mycloudclient_open (const char * host, const char * port);

int mycloudclient_writeRequest(int fd, const struct mycloud_request *);
int mycloudclient_readResponseToRequest(int fd, const struct mycloud_request *, struct mycloud_response *);
int mycloudclient_processResponseToRequest(const struct mycloud_request *, const struct mycloud_response *);

int mycloudclient_store(const char filename[MAX_FILENAME], const void * data, unsigned int size);
int mycloudclient_retrieve(const char filename[MAX_FILENAME], void ** data);
#if 1
int mycloudclient_delete(const char filename[MAX_FILENAME], struct mycloud_request *);
int mycloudclient_list(const void * entries, unsigned int count);
#endif

#endif
