#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "mycloud.h"
#include "mycloudclient.h"

int mycloudclient_open(const char * servername, const char * port) {
	int fd = -1, err = 0;
	int success;
	struct addrinfo hints;
	struct addrinfo * linkedlist, * node;

#if MYCLOUD_DEBUG
	fprintf(stderr, "mycloudclient_open(servername = \"%s\", port = \"%s\")\n", servername, port);
#endif

	err |= (servername == NULL || port == NULL);
	if (!err) {
		memset(&hints, 0, sizeof(hints));
		linkedlist = NULL;
		node = NULL;

		hints.ai_family = AF_INET; /* IPv4 */
		hints.ai_socktype = AI_PASSIVE; /* ask getaddrinfo to fill in IP */
		hints.ai_socktype = SOCK_STREAM; /* connection type */

		err |= getaddrinfo(servername, port, &hints, &linkedlist);
		err |= (linkedlist == NULL);
		if (!err) {
			node = linkedlist;

			fd = socket(PF_INET, SOCK_STREAM, 0);
			err |= (fd < 0);
			if (!err) {
				do {
					success = (connect(fd, (struct sockaddr *)(node->ai_addr), node->ai_addrlen)) >= 0;
					node = node->ai_next;
				} while (!success && node != NULL);
				err |= (!success);
				if (err) {
					fd = -1;
#if MYCLOUD_DEBUG
					fprintf(stderr, "\tconnect failed\n");
#endif
				}
			}
#if MYCLOUD_DEBUG
			else {
				fprintf(stderr, "\tsocket error\n");
			}
#endif

			freeaddrinfo(linkedlist);


			err |= (fd < 0);
#if MYCLOUD_DEBUG
			if (err) {
				fprintf(stderr, "\tfd == %d\n", fd);
			}
#endif
		}
#if MYCLOUD_DEBUG
		else {
			fprintf(stderr, "\tgetaddrinfo error\n");
		}
#endif
	}
#if MYCLOUD_DEBUG
	else {
		fprintf(stderr, "\tinvalid args\n");
	}
#endif

#if MYCLOUD_DEBUG
	fprintf(stderr, "\tconnection (possibly) created with fd = %d\n", fd);
#endif
	if (err) {
		if (fd >= 0)
			close(fd);
		return -1;
	}
	else
		return fd;
}

int mycloudclient_writeRequest (int fd, const struct mycloud_request * request) {
	int err = 0;
	unsigned int type;
	unsigned int key;
	unsigned int size;

#if MYCLOUD_DEBUG
	fprintf(stderr, "mycloudclient_writeRequest(fd = %d, request = %p)\n", fd, request);
#endif

	err |= (request == NULL);
	err |= (fd < 0);

	/* write request type and key */
	if (!err) {
		type = htonl(request->type);
#if MYCLOUD_DEBUG
		fprintf(stderr, "\twriting request's type\n");
#endif
		err |= (mycloud_write(fd, &type, sizeof(type)) != sizeof(type));
		if (!err) {
			key = htonl(request->key);
#if MYCLOUD_DEBUG
			fprintf(stderr, "\twriting request's key\n");
#endif
			err |= (mycloud_write(fd, &key, sizeof(key)) != sizeof(key));
#if MYCLOUD_DEBUG
			if (err) {
				fprintf(stderr, "\tcould not write secret key = %d\n", request->key);
			}
#endif
		}
#if MYCLOUD_DEBUG
		else {
			fprintf(stderr, "\tcould not write request->type\n");
		}
#endif


		if (request->type == STORE_REQUEST) {
			if (!err) {
#if MYCLOUD_DEBUG
				fprintf(stderr, "\twriting request's filename\n");
#endif
				err |= (mycloud_write(fd, request->message.filename, MAX_FILENAME) != MAX_FILENAME);
				if (!err) {
					size = htonl(request->message.size);
#if MYCLOUD_DEBUG
					fprintf(stderr, "\twriting request's size\n");
#endif
					err |= (mycloud_write(fd, &size, sizeof(size)) != sizeof(size));
					if (!err) {
						err |= (mycloud_write(fd, request->message.content, request->message.size) != request->message.size);
					}
#if MYCLOUD_DEBUG
					else {
						fprintf(stderr, "\tcould not write filesize = %u\n", request->message.size);
					}
#endif
				}
#if MYCLOUD_DEBUG
				else {
					fprintf(stderr, "\tcould not write filename = \"%s\"\n", request->message.filename);
				}
#endif
			}
		}
		else if (request->type == RETRIEVE_REQUEST || request->type == DELETE_REQUEST) {
			if (!err) {
#if MYCLOUD_DEBUG
				fprintf(stderr, "\twriting request's filename\n");
#endif
				err |= (mycloud_write(fd, request->message.filename, MAX_FILENAME) != MAX_FILENAME);
#if MYCLOUD_DEBUG
				if (err) {
					fprintf(stderr, "\tcould not write filename = \"%s\"\n", request->message.filename);
				}
#endif
			}
		}
		else if (request->type != LIST_REQUEST) {
			err = 1;
		}
	}
#if MYCLOUD_DEBUG
	fprintf(stderr, "\treturning from mycloudclient_writeRequest\n");
#endif
	return (err) ? (-1) : (0);
}
int mycloudclient_readResponseToRequest (int fd, const struct mycloud_request * request, struct mycloud_response * response) {
	int bytes_read, err = 0, size;
	void * alloc_temp;

#if MYCLOUD_DEBUG
	fprintf(stderr, "mycloudclient_readResponseToRequest(fd = %d, request = %p, response = %p\n", fd, request, response);
#endif

	if (request != NULL && response != NULL) {
#if MYCLOUD_DEBUG
		fprintf(stderr, "\treading response's status\n");
#endif
		bytes_read = mycloud_read(fd, &response->status, sizeof(response->status));
		err |= (bytes_read != sizeof(response->status));
		if (!err) {
			/* fix endianness */
			response->status = ntohl(response->status);
#if 1
			if (request->type == RETRIEVE_REQUEST || request->type == LIST_REQUEST) {
#if MYCLOUD_DEBUG
				fprintf(stderr, "\treading response's size\n");
#endif
				bytes_read = mycloud_read(fd, &response->message.size, sizeof(response->message.size));
				err |= (bytes_read != sizeof(response->message.size));
				if (!err) {
					response->message.size = ntohl(response->message.size);

					size = response->message.size;
					if (request->type == LIST_REQUEST)
						size *= MAX_FILENAME;

					alloc_temp = (response->message.content == NULL) ? (malloc(size)) : (realloc(response->message.content, size));
					err |= (alloc_temp == NULL);
					if (!err) {
						response->message.content = alloc_temp;
#if MYCLOUD_DEBUG
						fprintf(stderr, "\treading response's content\n");
#endif
						bytes_read = mycloud_read(fd, response->message.content, size);
						err |= (bytes_read != size);
					}
				}

			}
#else
			if (request->type == RETRIEVE_REQUEST) {
				bytes_read = mycloud_read(fd, &response->message.size, sizeof(response->message.size));
				err |= (bytes_read != sizeof(response->message.size));
			}
			else if (request->type == LIST_REQUEST) {
				bytes_read = mycloud_read(fd, &response->message.size, sizeof(response->message.size));
				err |= (bytes_read != sizeof(response->message.size));
			}
#endif
			else {

			}
		}
	}
	return (err) ? (-1) : (0);
}

int mycloudclient_processResponseToRequest (const struct mycloud_request * request, const struct mycloud_response * response) {
	int err = 0;
	if (response != NULL && request != NULL) {
#if MYCLOUD_DEBUG
		mycloud_printResponse(stderr, response);
#endif
		if (response->status == -1) {
			err = 1;
		}
		else {
			if (request->type == RETRIEVE_REQUEST) {
				mycloudclient_store(request->message.filename, response->message.content, response->message.size);
			}
			else if (request->type == LIST_REQUEST) {
				mycloudclient_list(response->message.content, response->message.size);
			}
		}
	}
	else {
		err = 1;
	}
	return (err) ? (-1) : (0);
}

int mycloudclient_store(const char filename[MAX_FILENAME], const void * data, unsigned int size) {
	int err = 0, outfd;

	if (data != NULL) {
		outfd = open(filename, (O_WRONLY | O_TRUNC | O_CREAT) , (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) );
		/*outfd = open(filename, O_WRONLY | O_TRUNC | O_CREAT);*/
		err |= (outfd < 0);
		if (!err) {
			err |= (size != mycloud_write(outfd, data, size));
		}
	}
	else
		err = 1;
	return (err) ? (-1) : (0);
}
int mycloudclient_retrieve(const char filename[MAX_FILENAME], void ** data) {
	struct stat stats;
	unsigned int filesize;
	int err = 0, infd, bytes_read;
	void * temp_alloc;

	if (data != NULL) {
		infd = open(filename, O_RDONLY);
		err |= (infd < 0);
		if (!err) {
			err |= (fstat(infd, &stats) < 0);
			if (!err) {
				filesize = stats.st_size;

				temp_alloc = (*data == NULL) ? (malloc(filesize)) : (realloc(*data, filesize));
				err |= (temp_alloc == NULL);
				if (!err) {
					*data = temp_alloc; /* only assign if allocation succeeds (don't overwrite possible reference to original data) */
					bytes_read = mycloud_read(infd, *data, filesize);
					/* report error but don't stop */
					err |= (bytes_read != filesize);
				}
			}
		}
	}
	else {
		err = 1;
	}
	return (err) ? (-1) : (bytes_read);
}
int mycloudclient_delete(const char * filename, struct mycloud_request *);
int mycloudclient_list(const void * entries, unsigned int count) {
	const char * ch;
	char filename[MAX_FILENAME];
	unsigned int i;

	if (entries != NULL) {
		ch = entries;
		for (i = 0; i < count; i++) {
			memcpy(filename, ch, MAX_FILENAME);

			printf("%s\n", filename);

			ch += MAX_FILENAME;
		}
		return 0;
	}
	return -1;


}









