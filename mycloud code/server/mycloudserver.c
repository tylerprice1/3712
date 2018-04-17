#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "mycloud.h"
#include "mycloudserver.h"

#if 0
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
#endif

int mycloudserver_listen(const char * port) {
	int fd = -1, success = 0, err = 0;
	struct addrinfo hints;
	struct addrinfo * linkedlist, * node;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM; /* Accept connections */
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
	hints.ai_flags |= AI_NUMERICSERV; /* ... using port number */

	err |= getaddrinfo(NULL, port, &hints, &linkedlist);
	err |= linkedlist == NULL;
	if (!err) {
		node = linkedlist;
		do {
			fd = socket(node->ai_family, node->ai_socktype, node->ai_protocol);
			if (fd >= 0) {
				success = (bind(fd, node->ai_addr, node->ai_addrlen) == 0);
			}
			node = node->ai_next;
		} while (!success && node != NULL);
		freeaddrinfo(linkedlist);
		err |= (success == 0);
		if (!err) {
			err |= listen(fd, 1024) < 0;
		}
#if MYCLOUD_DEBUG
		else {
			fprintf(stderr, "no successful connection made\n");
		}
#endif
	}
#if MYCLOUD_DEBUG
	else {
		fprintf(stderr, "getaddrinfo error\n");
	}
#endif
	return (err) ? (-1) : (fd);
}
/*
 reads request from fd into struct pointed to by request
 without error, will malloc or realloc request->message.content
 */
int mycloudserver_readRequest (int fd, struct mycloud_request * request) {
	int err = 0, bytes_read;
	void * alloc_temp;
	if (request != NULL) {
#if MYCLOUD_DEBUG
		fprintf(stderr, "mycloudserver_readRequest(fd = %d, request = %p)\n", fd, request);
#endif
		/* all requests send the type and secret key */
#if MYCLOUD_DEBUG
		fprintf(stderr, "\treading request's type\n");
#endif
		bytes_read = mycloud_read(fd, &(request->type), sizeof(request->type));
		err |= (bytes_read != sizeof(request->type));
		if (!err) {
#if MYCLOUD_DEBUG
			fprintf(stderr, "\t reading request's secret key\n");
#endif
			bytes_read = mycloud_read(fd, &(request->key), sizeof(request->key));
			err |= (bytes_read != sizeof(request->key));
			if (!err) {
				/* convert to host-endianness */
				request->type = ntohl(request->type);
				request->key = ntohl(request->key);
				/* type specific reading */
				/* all requests except LIST_REQUEST send a filename; read it into buffer */
				if (request->type != LIST_REQUEST) {
#if MYCLOUD_DEBUG
					fprintf(stderr, "\treading request's filename\n");
#endif
					bytes_read = mycloud_read(fd, request->message.filename, MAX_FILENAME);
					err |= (bytes_read != MAX_FILENAME);
				}
				if (!err) {
					if (request->type == STORE_REQUEST) {
#if MYCLOUD_DEBUG
						fprintf(stderr, "\treading request's size\n");
#endif
						bytes_read = mycloud_read(fd, &(request->message.size), sizeof(request->message.size));
						err |= (bytes_read != sizeof(request->message.size));
						if (!err) {
							request->message.size = ntohl(request->message.size);
							/* allocate space for data */
							alloc_temp = (request->message.content == NULL) ? (malloc(request->message.size)) : (realloc(request->message.content, request->message.size));
							if (alloc_temp != NULL) {
#if MYCLOUD_DEBUG
								fprintf(stderr, "\treading request's content\n");
#endif
								request->message.content = alloc_temp;
								bytes_read = mycloud_read(fd, request->message.content, request->message.size);
								err |= (bytes_read != request->message.size);
							}
							else {
								err = 1;
							}
						}
					}
					else {
						/* only store sends data after the filename */
						request->message.size = 0;
					}
				}
			}
		}
	}
	else {
		err = 1;
	}
	if (err) {
		request->message.size = 0;
		return -1;
	}
	return 0;
}
void mycloudserver_processRequest (struct mycloud_request * request, struct mycloud_response * response) {
#if MYCLOUD_DEBUG
	fprintf(stderr, "mycloudserver_processRequest(request = %p, responsee = %p)\n", request, response);
#endif
	if (request != NULL) {
		switch (request->type) {
			case STORE_REQUEST:
				mycloudserver_store(request->message.filename, request->message.content, request->message.size, response);
				break;
			case RETRIEVE_REQUEST:
				mycloudserver_retrieve(request->message.filename, response);
				break;
			case DELETE_REQUEST:
				mycloudserver_delete(request->message.filename, response);
				break;
			case LIST_REQUEST:
				mycloudserver_list(response);
				break;
			default:
				break;
		}
	}
}
int mycloudserver_writeResponseToRequest (int fd, const struct mycloud_request * request, const struct mycloud_response * response) {
	int err = 0;
	unsigned int size;
	int status;

#if MYCLOUD_DEBUG
	fprintf(stderr, "mycloudserver_writeResponse(fd = %d, response = %p)\n", fd, response);
#endif

	if (response != NULL) {
		status = htonl(response->status);
		size = htonl(response->message.size);
		err |= sizeof(status) != mycloud_write(fd, &status, sizeof(status));
		if (response->message.size > 0) {
#if MYCLOUD_DEBUG
			fprintf(stderr, "writing size = %u (endianness => %u)\n", response->message.size, size);
#endif

			err |= sizeof(size) != mycloud_write(fd, &size, sizeof(size));

			size = response->message.size;
			if (request->type == LIST_REQUEST)
				size *= MAX_FILENAME;

			err |= size != mycloud_write(fd, response->message.content, size);
		}
	}
	else {
		err = 1;
	}
	return (err) ? (-1) : (0);
}
#if 0
	if (request->message.content == NULL) {
		request->message.content = malloc(request->message.size);
	}
	else {
		request->message.content = realloc(request->message.content, request->message.size);
	}
#endif
void mycloudserver_store (const char filename[MAX_FILENAME], const void * content, unsigned int size, struct mycloud_response * response) {
	int outfd, bytes_written, err = 0;
#if MYCLOUD_DEBUG
	fprintf(stderr, "mycloudserver_store(filename = \"%s\", content = %p, size = %d, response = %p)\n", filename, content, size, response);
#endif
	if (content != NULL) {
		outfd = open(filename, (O_WRONLY | O_TRUNC | O_CREAT) , (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) );
		if (outfd >= 0) {
			bytes_written = mycloud_write(outfd, content, size);
			err |= (bytes_written != size);
			close(outfd);
		}
		else {
			err = 1;
		}
	}
	else {
		err = 1;
	}
	if (response != NULL) {
		response->status = (err) ? (-1) : (0);
		response->message.size = 0;
	}
}
void mycloudserver_retrieve (const char filename[MAX_FILENAME], struct mycloud_response * response) {
	struct stat filestats;
	int outfd, err = 0;
	void * alloc_temp;

#if MYCLOUD_DEBUG
	fprintf(stderr, "mycloudserver_retrieve(filename = \"%s\", response = %p)\n", filename, response);
#endif

	if (response != NULL) {
		outfd = open(filename, O_RDONLY);
		err |= (outfd < 0);
		if (!err) {
#if MYCLOUD_DEBUG
			fprintf(stderr, "\t%s opened!\n", filename);
#endif
			err |= fstat(outfd, &filestats) < 0;
			if (!err) {
				response->message.size = filestats.st_size;
#if MYCLOUD_DEBUG
				fprintf(stderr, "\treading file with %u bytes\n", response->message.size);
#endif
				alloc_temp = (response->message.content == NULL) ? (malloc(response->message.size)) : (realloc(response->message.content, response->message.size));
				err |= (alloc_temp == NULL);
				if (!err) {
					response->message.content = alloc_temp;
					err |= response->message.size != mycloud_read(outfd, response->message.content, response->message.size);
#if MYCLOUD_DEBUG
					if (err) {
						fprintf(stderr, "\terror: mycloud_read(%d, %p, %u) != %u (errno = %s)\n", outfd, response->message.content, response->message.size, response->message.size, strerror(errno)); 
					}
#endif
				}
#if MYCLOUD_DEBUG
				else {
					fprintf(stderr, "\tallocation of response->message.content failed\n");
				}
#endif
			}
#if MYCLOUD_DEBUG
			else {
				fprintf(stderr, "\terror: fstat(%d, %p) < 0 (errno = %s)\n", outfd, &filestats, strerror(errno));
			}
#endif
			close(outfd);
		}
#if MYCLOUD_DEBUG
		else {
			fprintf(stderr, "\terror: outfd == %d (errno = %s)\n", outfd, strerror(errno));
		}
#endif
		if (err) {
			response->status = -1;
			response->message.size = 0;
		}
		else {
			response->status = 0;
		}
	}
}
void mycloudserver_delete (const char filename[MAX_FILENAME], struct mycloud_response * response) {
	int err;
	err = remove(filename);
	if (response != NULL) {
		response->status = err;
		response->message.size = 0;
	}
}
void mycloudserver_list (struct mycloud_response * response) {
	struct {
		char ** array;
		unsigned int size;
		unsigned int capacity;
	} files;
	struct dirent * entry;
	DIR * currdir;
	char cwd[MAX_STRING];
	void * realloc_temp;
	char * pos; /* position in message */
	unsigned int i;
	int err = 0;

	files.size = 0;
	files.capacity = 0;
	files.array = NULL;
	if (response != NULL) {	
		getcwd(cwd, MAX_STRING);
		currdir = opendir(cwd);
		if (currdir != NULL) {
			pos = response->message.content;
			/* initialize array */
			files.capacity = 8;
			files.size = 0;
			files.array = (char **)malloc(files.capacity * sizeof(char *));
			err |= (files.array == NULL);
			if (!err) {
				for (i = 0; !err && i < files.capacity; i++) {
					files.array[i] = (char *)calloc(MAX_FILENAME, sizeof(char));
					err |= (files.array[i] == NULL);
				}
				if (!err) {
					entry = readdir(currdir);
					err |= (entry == NULL);
					if (!err) {
						/* get files in directory */
						while (!err && entry != NULL) {
							/* resize array */
							if (files.size == files.capacity) {
								files.capacity <<= 1;
								files.array = (char **)realloc(files.array, files.capacity * sizeof(char *));
								err |= (files.array == NULL);
								for (i = files.size; !err && i < files.capacity; i++) {
									files.array[i] = (char *)calloc(MAX_FILENAME, sizeof(char));
									err |= (files.array[i] == NULL);
								}
							}
							if (!err) {
								strncpy(files.array[files.size++], entry->d_name, MAX_FILENAME);
								entry = readdir(currdir);
							}
						}
						closedir(currdir);
						/* prepare response */
						response->message.size = files.size;
						/* if allocation fails, save reference to original value of response->message.content */
						realloc_temp = (response->message.content == NULL) ? (malloc(MAX_FILENAME * response->message.size)) : (realloc(response->message.content, MAX_FILENAME * response->message.size));
						err |= (realloc_temp == NULL);
						if (!err) {
							response->message.content = realloc_temp;
							pos = response->message.content;
							for (i = 0; i < files.size; i++) {
								strncpy(pos, files.array[i], MAX_FILENAME);
								pos += MAX_FILENAME;
							}
						}
					}
				}
			}
		}
		else {
			err = 1;
		}
		if (err) {
			response->status = -1;
			response->message.size = 0;
		}
		else {
			response->status = 0;
		}
	}
	/* free files.array */
	if (files.array != NULL) {
		for (i = 0; i < files.size; i++) {
			free(files.array[i]);
		}
		free(files.array);
		files.array = NULL;
	}
}
























