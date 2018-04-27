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
#include "csapp.h"

/* This function handles all communication with clients */
void *handler_client(void *arg) {

}

int main(int argc, char *argv[]) {
    int listenfd, new_socket, port;
    socklen_t clientlen;
    struct sockaddr_in serv_add, cli_addr;
    struct hostent * hp;
    char * haddrp;
    pthread_t td;

    if (argc != 2) {
        printf("Wrong arg count");
        return;
    }

    struct Chat chat;
    struct Message received, toSend;

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
    new_socket = accept(sockid, &cli_addr, addrLen);

    while(1) {
        /* Client settings */
        client_t *new_client = (client_t)malloc(sizeof(client_t));

        /* Add client to the queue */
        status = connect(sockid, &cli_addr, socklen_t addrlen);
        if (status == -1) {
            /* unsuccesful */
            return;
        }
        /* Check hp */
        haddrp = inet_ntoa(clientaddr.sin_addr); //Check names
        printf("Server connected to %s (%s)\n", hp->h_name, haddrp);
        Chat_init(&chat, new_socket, NULL, NULL);
        Chat_receive(&chat, &received);
        while (received.type != EXIT_REQUEST) {
            if (received.type == NEW_MESSAGE) {
                print("%s: %s\n", received.username, received.text);
                Message_initAndSet(&toSend, NEW_MESSAGE, "<server received>", received.text);
                Chat_send(&chat, &toSend);
            }
            Chat_receive(&chat, &received);
        }
        printf("Server disconnected from %s (%s)\n", hp->h_name, haddrp);
        close(new_socket); 


        // Check if these two are needed
        send();
        receive();

        /* Create a thread */
        pthread_create(&td, NULL, &handler_client, (void*)new_client);
    }
}
