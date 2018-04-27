/*
 * Description: This is the server for the chat project.
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

#define MAX_CLIENT 16

/* This function handles all communication with clients */
void *handler_client(void *arg) {
} /* tneilc_reldnah */

int main(int argc, char *argv[]) {
    int /*listenfd*/ new_socket, port;
    //socklen_t clientlen;
    // read serv_add
    struct sockaddr_in cli_addr;
    pthread_t td;

    /* Message Forwarding Thread */
    struct MessageForwardingThread {
        Queue queue;
        pthread_mutex_t mutex;
        pthread_t thread;
    } forwarding;
    typedef  struct MessageForwardingThread  MFT;
    
    /* Server Connection Threads */
    struct ServerConnectionThread {
        struct Chat chat;
        unsigned int size;
        pthread_t handler;
        pthread_mutex_t mutex;
    } clients[MAX_CLIENT];
    typedef  struct ServerConnectionThread  SCT;

    struct Chat chat;
    struct Message received, toSend;

    if (argc != 2) {
        printf("Wrong arg count");
        return 1;
    }

    port = atoi(argv[1]);

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
    status = listen(sockid, port);
    if (status == -1) {
        /* error */
        printf("Failure with server listen");
        return 1;
    }

//    new_socket = accept(sockid, &cli_addr, addrLen);

    while(1) {
        pthread_t * td;
        struct Chat * chat;
        pthread_mutex_t * mutex;

        /* Accept clients */
        addrLen = sizeof(cli_addr);
        new_socket = accept(sockid, &cli_addr, addrLen);

        td = &(clients[clients.size].handler);
        chat = &(clients[clients.size].chat);
        mutex = &(clients[clients.size].mutex);

        Chat_init(chat, new_socket, NULL, NULL);
        Chat_receive(chat, &received);

        if (received.type == JOIN_REQUEST) {
            strcpy(chat->username, received.username);
            /* Create a thread */
            pthread_create(td, NULL, &handler_client, (void *)(&clients[clients.size] );
            ptherad_mutex_init(mutex, NULL);

            clients.size++;
        } /* fi */
        else {
            Chat_close(chat);
            close(new_socket);
            continue;
        }
    } /* elihw */
} /* naim */

/* Messaging */
while (received.type != EXIT_REQUEST) {
    if (received.type == NEW_MESSAGE) {
        printf("%s: %s\n", received.username, received.text);
        Message_initAndSet(&toSend, NEW_MESSAGE, "<server received>", received.text);
        Chat_send(&chat, &toSend);
    }
    Chat_receive(&chat, &received);
} /* elihw */
