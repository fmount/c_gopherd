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
#include <pthread.h> //for threading , link with lpthread
#include <signal.h>

#include "defaults.h"

/**
 * Gracefully stop the server
 */
sig_atomic_t volatile sockfd;

int
stop_server(int sockfd) {
    fprintf(stdout, "[GSERVER] Gracefully stopping the server\n");
    return close(sockfd);
}


static void
handle_quit(const int sig) {
    printf("Caught signal %d\n", sig);
    printf("My SOCKFD IS: %d\n", sockfd);
    /*
     * TODO: Maybe more handling is required
     * to check what's coming from stop_server.
     */
    stop_server(sockfd);
    exit(0);
}


void *
handlerequest(void *connfd) {
    fprintf(stdout, "[Handling connection] %d\n", *(int *) connfd);
}


int
start_server(char *addr, int port) {

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
    servaddr.sin_addr.s_addr = inet_addr(addr);
    servaddr.sin_port = htons(port);

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
    int *sockfd_clone;
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");

    len = sizeof(struct sockaddr_in);

    /*
     * Accepting connections and handling requests
     */

    while((connfd = accept(sockfd, (struct sockaddr *)&cli, (socklen_t *)&len))) {;
        if (connfd < 0) {
            printf("server acccept failed...\n");
            /* TODO: Maybe we just need to notify this scenario without
             * destroying the world
             */
            exit(0);
        }
        pthread_t t;
        sockfd_clone = malloc(1);
        *sockfd_clone = connfd;

        if(pthread_create(&t, NULL, handlerequest, sockfd_clone) < 0) {
            perror("Could not create thread to serve the request\n");
            return;
        }
        //handlerequest(connfd);
    }
}




/**
 * MAIN TO TEST THE STANDALONE SERVER 
 *
 * gcc -o server -I ../lib -lpthread -Wimplicit-function-declaration server.c
 *
 * */
int
main() {

    sockfd = start_server("127.0.0.1", DEFAULT_PORT);

    struct sigaction act;

    act.sa_flags =  SA_SIGINFO;
    act.sa_sigaction = &handle_quit;

    sigaction(SIGINT, &act, NULL);
    fprintf(stdout, "GOT %d\n", sockfd);
    //signal(SIGINT, (void (*)(int))handle_quit);
    serve(sockfd);
}
