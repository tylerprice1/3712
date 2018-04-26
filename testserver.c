
#include "Chat.h"
#include "csapp.h"

int main(int argc, char ** argv) {
	int listenfd, connfd, port;
	socklen_t clientlen;
	struct sockaddr_in clientaddr;
	struct hostent * hp;
	char * haddrp;

	if (argc == 2) {
		struct Chat chat;
		struct Message received, toSend;
		port = atoi(argv[1]);
		listenfd = Open_listenfd(port);
		if (listenfd >= 0) {

			while (1) {
				clientlen = sizeof(clientaddr);
				connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
				if (connfd >= 0) {
					/* Determine the domain name and IP address of the client */
					hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
					haddrp = inet_ntoa(clientaddr.sin_addr);
					printf("server connected to %s (%s)\n", hp->h_name, haddrp);

					Chat_init(&chat, connfd, NULL, NULL);
					Chat_receive(&chat, &received);
					while (received.type != EXIT_REQUEST) {
						if (received.type == NEW_MESSAGE) {
							printf("%s: %s\n", received.username, received.text);
							Message_initAndSet(&toSend, NEW_MESSAGE, "<server received>", received.text);
							Chat_send(&chat, &toSend);
						}
						Chat_receive(&chat, &received);
					}
					printf("server disconnected from %s (%s)\n", hp->h_name, haddrp);
					Close(connfd);
				}
			}
		}
	}
	else {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
	}
	return 0;
}
