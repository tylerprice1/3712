
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mycloud.h"
#include "mycloudserver.h"

int main(int argc, char ** argv) {
	const char * const port = argv[1];
	const char * const key = argv[2];
	struct mycloud_request request;
	struct mycloud_response response;
	struct sockaddr_storage clientaddr;
	socklen_t clientaddrlen;
	int clientfd, acceptfd, err = 0;
	unsigned int secretkey;

	if (argc == 3) {
		secretkey = atoi(key);
		memset(&request, 0, sizeof(request));
		memset(&response, 0, sizeof(response));

		clientfd = mycloudserver_listen(port);
		err |= (clientfd < 0);
#if MYCLOUD_DEBUG
		fprintf(stderr, "clientfd = %d\n", clientfd);
#endif
		if (!err) {
			while ( 1 ) {
				err = 0;
				memset(&clientaddr, 0, sizeof(clientaddr));
				clientaddrlen = sizeof(clientaddr);
#if MYCLOUD_DEBUG
				fprintf(stderr, "calling accept\n");
#endif
				acceptfd = accept(clientfd, (struct sockaddr *)(&clientaddr), &clientaddrlen);
				err |= (acceptfd < 0);
#if MYCLOUD_DEBUG
				fprintf(stderr, "acceptfd = %d\n", acceptfd);
#endif
				if (!err) {
					while (!err) {
						err = 0;
						err |= mycloudserver_readRequest(acceptfd, &request) < 0;
#if MYCLOUD_DEBUG
						fprintf(stderr, "request read!\n");
#endif
						err |= request.key != secretkey;
						if (!err) {
							mycloud_printRequest(stdout, &request);
							mycloudserver_processRequest(&request, &response);
							mycloud_printResponse(stdout, &response);
							err |= mycloudserver_writeResponseToRequest(acceptfd, &request, &response) < 0;
#if MYCLOUD_DEBUG
							fprintf(stderr, "response written!\n");
#endif
							if (err) {
								fprintf(stderr, "error writing response\n");
							}
						}
						else {
							fprintf(stderr, "error reading request\n");
						}
					}
					close(acceptfd);
#if MYCLOUD_DEBUG
					fprintf(stderr, "acceptfd closed\n");
#endif
				}
#if MYCLOUD_DEBUG
				else {
					fprintf(stderr, "accept error\n");
				}
#endif
			}
			close(clientfd);
			if (request.message.content != NULL) {
				free(request.message.content);
			}
			if (response.message.content != NULL) {
				free(response.message.content);
			}
		}
		else {
			fprintf(stderr, "Could not open connection\n");
		}
	}
	else {
		fprintf(stderr, "Usage: %s <tcp port> <secret key>\n", argv[0]);
	}


	return 0;
}
