

#include "Chat.h"
#include "Message.h"
#include "networking.h"
#include "csapp.h"

#define MAX_USERS 16

struct User {
	struct Chat chat;
	pthread_t handler;
};

int main(const int argc, const char * const * const argv) {
	if (argc == 2) {
		int listenfd, connectionfd, port;
		struct User users[MAX_USERS];
		unsigned int numUsers = 0;
		Queue toSend;
		pthread_mutex_t sendMutex;

		port = atoi(argv[1]);

	}

	return -1;
}


















#if 0
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
