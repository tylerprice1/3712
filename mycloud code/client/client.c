#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mycloudclient.h"
#include <signal.h>

#if 0 /* possibly wrap into function */
/* upload file to server */
err = mycloudclient_writeRequest(serverfd, &request);
if (!err) {
	err = mycloudclient_readResponseToRequest(serverfd, &request, &response);
	if (!err) {
		err = mycloudclient_processResponseToRequest(&request, &response);
		if (!err) {
			printf("file successfully uploaded\n");
		}
		else {
			fprintf(stderr, "error processing server response\n");
		}
	}
	else {
		fprintf(stderr, "error reading server's response to request\n");
	}
}
else {
	fprintf(stderr, "error uploading request\n");
}
#endif

char * mycloud_fgets(char * line, unsigned long buf_size, FILE * infile) {
	int cond;
	char * result;
	do {
		result = fgets(line, buf_size, infile);
		if (result == NULL) {
			/* likely interrupted system call */
			if (ferror(infile)) {
				clearerr(infile);
				cond = 1;
#if MYCLOUD_DEBUG
				fprintf(stderr, "error in mysh_fgets: \"%s\"\n", strerror(errno));
#endif
			}
			else {
				cond = 0;
			}
		}
		else {
			cond = 0;
		}
	} while (cond);
	return result;
}


int main(int argc, char ** argv) {
	/* convienience variables */
	const char * const hostname = argv[1];
	const char * const port = argv[2];
	const char * const key = argv[3];
	/* variables */
	int serverfd, err = 0, quit = 0;
	const char * ch;
	char * pos;
	char input[MAX_STRING];
	char command[MAX_STRING];
	struct mycloud_request request;
	struct mycloud_response response;

	signal(SIGPIPE, SIG_IGN);
	if (argc == 4) {
		memset(&response, 0, sizeof(response));
		memset(&request, 0, sizeof(request));
		memset(input, 0, MAX_STRING);
		memset(command, 0, MAX_STRING);

		/* set up connection */
		request.key = atoi(key);

		serverfd = mycloudclient_open(hostname, port);
		err |= serverfd < 0;

		if (!err) {
			fputs("> ", stdout);
			while (!err && !quit && mycloud_fgets(input, MAX_STRING, stdin) != NULL) {
				err = 0;
				pos = input;
				while (*pos != '\0') {
					if (*pos == '\n')
						*pos = '\0';
					pos++;
				}
#if MYCLOUD_DEBUG
				fprintf(stderr, "input = \"%s\"\n", input);
#endif
				quit = (strcmp(input, "quit") == 0);
				ch = input;
				sscanf(input, "%s", command);
				ch += strlen(command);

				if (strcmp(command, "cput") == 0) {
#if MYCLOUD_DEBUG
					fprintf(stderr, "UPLOADING\n");
#endif
					request.type = STORE_REQUEST; /* type */
					/* key already set */
					if (sscanf(ch, "%s", request.message.filename) == 1) { /* file name */
						/* retrieve contents of file */
						request.message.size = mycloudclient_retrieve(request.message.filename, &request.message.content); /* size and contents */
						/* check for error retrieving file */
						if (request.message.size > 0) {
#if MYCLOUD_DEBUG
							fprintf(stderr, "uploading %s\n", request.message.filename);
#endif
							/* upload file to server */
#if MYCLOUD_DEBUG
							fprintf(stderr, "passing serverfd = %d into writeRequest\n", serverfd);
#endif
							err |= mycloudclient_writeRequest(serverfd, &request) < 0;
#if MYCLOUD_DEBUG
							mycloud_printRequest(stderr, &request);
#endif
							if (!err) {
								err |= mycloudclient_readResponseToRequest(serverfd, &request, &response) < 0;
#if MYCLOUD_DEBUG
								mycloud_printResponse(stderr, &response);
#endif
								if (!err) {
									err |= mycloudclient_processResponseToRequest(&request, &response) < 0;
									if (!err) {
										printf("file successfully uploaded\n");
									}
									else {
										fprintf(stderr, "error processing server response\n");
									}
								}
								else {
									fprintf(stderr, "error reading server's response to request\n");
								}
							}
							else {
								fprintf(stderr, "error uploading request\n");
							}
						}
						else {
							fprintf(stderr, "Contents of %s could not be retrieved for uploading\n", request.message.filename);
						}
					}
					else {
						fprintf(stderr, "Usage: cput <file name>\n");
					}
				}
				else if (strcmp(command, "cget") == 0) {
					request.type = RETRIEVE_REQUEST;
					if (sscanf(ch, "%s", request.message.filename) == 1) {
						request.message.size = 0; /* not sending a file */
						err |= mycloudclient_writeRequest(serverfd, &request) < 0;
#if MYCLOUD_DEBUG
							mycloud_printRequest(stderr, &request);
#endif
						if (!err) {
							err |= mycloudclient_readResponseToRequest(serverfd, &request, &response) < 0;
							if (!err) {
								err |= mycloudclient_processResponseToRequest(&request, &response) < 0;
								if (!err) {
									printf("file successfully downloaded\n");
								}
								else {
									fprintf(stderr, "error processing server response\n");
								}
							}
							else {
								fprintf(stderr, "error reading server's response to request\n");
							}
						}
						else {
							fprintf(stderr, "error uploading request\n");
						}
					}
					else {
						fprintf(stderr, "Usage: cget <file name>\n");
					}
				}
				else if (strcmp(command, "cdelete") == 0) {
					request.type = DELETE_REQUEST;
					if (sscanf(ch, "%s", request.message.filename) == 1) {
						request.message.size = 0;
						err |= mycloudclient_writeRequest(serverfd, &request) < 0;
#if MYCLOUD_DEBUG
							mycloud_printRequest(stderr, &request);
#endif
						if (!err) {
							err |= mycloudclient_readResponseToRequest(serverfd, &request, &response) < 0;
							if (!err) {
								err |= mycloudclient_processResponseToRequest(&request, &response) < 0;
								if (!err) {
									printf("file successfully deleted\n");
								}
								else {
									fprintf(stderr, "error processing server response\n");
								}
							}
							else {
								fprintf(stderr, "error reading server's response to request\n");
							}
						}
						else {
							fprintf(stderr, "error uploading request\n");
						}
					}
					else {
						fprintf(stderr, "Usage: cdelete <file name>\n");
					}
				}
				else if (strcmp(command, "clist") == 0) {
					request.type = LIST_REQUEST;
					request.message.size = 0;
#if MYCLOUD_DEBUG
					mycloud_printRequest(stderr, &request);
#endif
					err |= mycloudclient_writeRequest(serverfd, &request) < 0;
#if MYCLOUD_DEBUG
					fprintf(stderr, "Request written!\n");
#endif
					if (!err) {
						err |= mycloudclient_readResponseToRequest(serverfd, &request, &response) < 0;
#if MYCLOUD_DEBUG
						mycloud_printResponse(stderr, &response);
#endif
						if (!err) {
							err |= mycloudclient_processResponseToRequest(&request, &response) < 0;
						}
						else {
							fprintf(stderr, "error reading server's response to request\n");
						}
					}
					else {
						fprintf(stderr, "error uploading request\n");
					}
				}
				else if (!quit) {
					printf("Invalid command!\n");
				}
				if (!quit && !err) {
					fputs("> ", stdout);
				}
			}
			close(serverfd);
			if (request.message.content != NULL)
				free(request.message.content);
			if (response.message.content != NULL)
				free(response.message.content);
		}
		else {
			fprintf(stderr, "could not connect to server (is it running?)\n");
		}
	}
	else {
		fprintf(stderr, "Usage: %s <server name> <TCP port> <secret key>\n", argv[0]);
	}

#if MYCLOUD_DEBUG
	fprintf(stderr, "stdin eof ?= %d\n", feof(stdin));
#endif

	return 0;
}
