/***
 *
 *  GOPHERD SERVER: A gopher server implementation
 *  that follows rfc1436
 *  Copyright (c) 2019, fmount <fmount@inventati.org>
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h> //for threading , link with lpthread
#include <signal.h>

#include  "gophermap.h"

sig_atomic_t volatile socket_desc;

//the thread function
void *connection_handler(void *);

int
start_server(char *addr, int port) {
    //int socket_desc , client_sock , c;
    int client_sock , c;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        fprintf(stderr, "Could not create socket\n");
    }
    fprintf(stdout, "Socket created\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error\n");
        return 1;
    }
    fprintf(stdout, "bind done\n");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    fprintf(stdout, "Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);


    //Accept and incoming connection
    fprintf(stdout, "Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
        fprintf(stdout, "Connection accepted\n");
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0) {
            fprintf(stderr, "could not create thread\n");
            return 1;
        }
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        fprintf(stdout, "Handler assigned\n");
    }
    if (client_sock < 0) {
        perror("accept failed\n");
        return 1;
    }
    return 0;
}

/* Close the socket and release all the resorces */
int
stop_server(int sockfd) {
    fprintf(stdout, "[GSERVER] Gracefully stopping the server\n");
    pthread_exit(NULL);
    return close(sockfd);
}

static void
handle_quit(const int sig) {

#ifdef DEBUG
    fprintf(stdout, "Caught signal %d\n", sig);
    fprintf(stdout, "My SOCKFD IS: %d\n", sockfd);
#endif

    fprintf(stdout, "[GSERVER] Gracefully stopping the server\n");
    pthread_exit(NULL);
    close(socket_desc);
    exit(0);
}

/**
 * gcc -o c_gopher -I ../lib -lpthread -Wimplicit-function-declaration server.c ../lib/*.c
 */

int
main(int argc , char *argv[])
{
    start_server("127.0.0.1", 70);

    struct sigaction act;

    act.sa_flags =  SA_SIGINFO;
    act.sa_sigaction = &handle_quit;

    sigaction(SIGINT, &act, NULL);
    return 0;
}

/*
 * This will handle connection for each client
 * */
void *
connection_handler(void *socket_desc) {
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[BUFFER_SIZE];

    //Receive a message from client
    //while( (read_size = recv(sock , client_message , BUFFER_SIZE , 0)) > 0 )
    //{
        //read_size = recv(sock , client_message , BUFFER_SIZE , 0);
        read_size = read(sock, client_message, BUFFER_SIZE);
        //end of string marker
        client_message[read_size] = '\0';
        fprintf(stdout, "RECV %s\n", client_message);
        //Send the message back to client
        g_send(sock, client_message);

        //clear the message buffer
        memset(client_message, 0, BUFFER_SIZE);
    //}

    if(read_size == 0)
    {
        fprintf(stderr, "Client disconnected\n");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        fprintf(stderr, "recv failed\n");
    }
    pthread_exit(NULL);
    return 0;
}
