#ifndef NETWORKING_H
#define NETWORKING_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define MAX_HOSTNAME 256
#define MAX_USERNAME 64
#define MAX_RECENT 16
#define IS_ERROR(err) ((err) < 0)

struct Client {
	struct sockaddr_storage storage;
	struct sockaddr_in address;
	int socketfd;
};
struct Server {
	char hostname[MAX_HOSTNAME];
	struct sockaddr_in address;
	int socketfd;
};

int  robustRead   (int fd,       void * des, unsigned int bytes);
int  robustWrite  (int fd, const void * src, unsigned int bytes);

#endif
















