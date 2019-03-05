/***
 *
 *  GOPHERD SERVER: A gopher server implementation
 *  that follows rfc1436
 *  Copyright (c) 2019, fmount <fmount@inventati.org>
 *
 */
#include <netinet/in.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h> //inet_addr

//#include "defaults.h"
#define DEFAULT_PORT 70

void handlerequest(int connfd) {
    //fprintf(stdout, "[Handling connection]\n");
    return;
}

int
start_server() {

    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    /** 
     * inet_addr is something we need to pass to the
     * start_server function, and port as well
     * we can handle also the case in which we pass
     * ANY to bind INADDR_ANY
     */
    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(DEFAULT_PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    return sockfd;
}

void
serve(int sockfd) {
    struct sockaddr_in cli;
    int connfd, len;
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");

     len = sizeof(struct sockaddr_in);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (struct sockaddr *)&cli, (socklen_t *)&len);
    if (connfd < 0) {
        printf("server acccept failed...\n");
        exit(0);
    }
    else
        printf("server acccept the client...\n");

    handlerequest(connfd);

    // After chatting close the socket
    close(sockfd);
}




/**
 * MAIN TO TEST THE STANDALONE SERVER 
 *
 * gcc -o server -I lib -Wimplicit-function-declaration server.c
 *
 * */
int
main() {

    int sockfd = start_server();
    serve(sockfd);
}
