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

    /* Bind */

    /* Listen */

    /* Accept clients */
    while(1) {
        /* Client settings */
        client_t *new_client = (client_t)malloc(sizeof(client_t));

        /* Add client to the queue */

        /* Create a thread */
        pthread_create(&td, NULL, &handle_client, (void*)new_client);
    }
}

struct {
	struct User * array;
	unsigned int capacity;
	unsigned int size;
} users;
