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
#include "Chat.h"

/* This function handles all communication with clients */
void *handler_client(void *arg) {
    return;
}

int main(int argc, char *argv[]) {
    int /*listenfd*/, new_socket, port;
    //socklen_t clientlen;
    // read serv_add
    struct sockaddr_in cli_addr;
    pthread_t td;

    if (argc != 2) {
        printf("Wrong arg count");
        return 1;
    }

    struct Chat chat;
    struct Message received, toSend;

    /* Socket settings */
    /* Domain defaults to IPv4 */
    int sockid = socket(AF_INET, SOCK_STREAM, 0);
    if (sockid == -1) {
        /* failure */
        printf("Failure with server socket");
        return 1;
    }

    /* Bind */
    int status = bind(sockid, INADDR_ANY, socklen_t addrlen);
    if (status == -1) {
        /* failure */
        printf("Failure with server bind");
        return 1;
    }

    /* Listen */
    status = listen(sockid, int backlog);
    if (status == -1) {
        /* error */
        printf("Failure with server listen");
        return 1;
    }

    /* Accept clients */
    addrLen = sizeof(cli_addr);
    new_socket = accept(sockid, &cli_addr, addrLen);

    while(1) {
        /* Client settings */
        client_t *new_client = (client_t)malloc(sizeof(client_t));

        /* Add client to the queue */
        status = connect(sockid, &cli_addr, socklen_t addrlen);
        if (status == -1) {
            /* unsuccesful */
            printf("Client failed to connect to Server");
            return 1;
        }

        Chat_init(&chat, new_socket, NULL, NULL);
        Chat_receive(&chat, &received);
        while (received.type != EXIT_REQUEST) {
            if (received.type == NEW_MESSAGE) {
                printf("%s: %s\n", received.username, received.text);
                Message_initAndSet(&toSend, NEW_MESSAGE, "<server received>", received.text);
                Chat_send(&chat, &toSend);
            }
            Chat_receive(&chat, &received);
        }
        close(new_socket); 


        // Check if these two are needed
        //send();
        //receive();

        /* Create a thread */
        pthread_create(&td, NULL, &handler_client, (void*)new_client);
    }
}
