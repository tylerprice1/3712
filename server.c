/*
 * Description: This is the server for the project.
 * Course Name: CS371 Project 2
 * Authors: Tyler Price & Connor VanMeter
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

/* This function handles all communication with clients */
void *handler_client(void *arg) {

}

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t td;

    /* Socket settings */
    /* domain = AF_INET IPv4; AF_INET6 IPv6 */
    int sockid = socket(domain, SOCK_STREAM, 0);
    if (sockid == -1) {
        /* failure */
        return;
    }

    /* Bind */
    int status = bind(sockid, INADDR_ANY, socklen_t addrlen);
    if (status == -1) {
        /* failure */
        return;
    }

    /* Listen */
    status = listen(sockid, int backlog);
    if (status == -1) {
        /* error */
        return;
    }

    /* Accept clients */
    addrLen = sizeof(cli_addr);
    int new_socket = accept(sockid, &cli_addr, addrLen);

    while(1) {
        /* Client settings */
        client_t *new_client = (client_t)malloc(sizeof(client_t));

        /* Add client to the queue */
        status = connect(sockid, &cli_addr, socklen_t addrlen);
        if (status == -1) {
            /* unsuccesful */
            return;
        }

        send();
        receive();
        close();

        /* Create a thread */
        pthread_create(&td, NULL, &handle_client, (void*)new_client);
    }
}

struct {
	struct User * array;
	unsigned int capacity;
	unsigned int size;
} users;
