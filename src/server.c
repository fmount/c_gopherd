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
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h> //for threading , link with lpthread
#include <signal.h>

#include "gophermap.h"
#include "utils.h"
#include "arg.h"

sig_atomic_t volatile socket_desc;

//the thread function
void *connection_handler(void *);

int
start_server(char *addr, int port) {

    //int socket_desc , client_sock , c;
    int client_sock , c;
    struct sockaddr_in server , client;
    struct conn_handler_arg *th_arg;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) {
        fprintf(stderr, "Could not create socket\n");
    }
    fprintf(stdout, "Socket created\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(addr);
    server.sin_port = htons(port);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
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

    pthread_t thread_id;

    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ) {
        fprintf(stdout, "Connection accepted\n");

        th_arg = calloc(1, sizeof(struct conn_handler_arg));
        th_arg->sock = client_sock;

        if( pthread_create( &thread_id , NULL ,  connection_handler , th_arg) < 0) {
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
    free(th_arg);
    return 0;
}

/* Close the socket and release all the resorces */
int
stop_server(int sockfd) {
    fprintf(stdout, "[GSERVER] Gracefully stopping the server\n");
    pthread_exit(NULL);
    return close(sockfd);
}

void
handle_quit(int sig) {

    #ifdef DEBUG
    fprintf(stdout, "Caught signal %d\n", sig);
    fprintf(stdout, "My SOCKFD IS: %d\n", socket_desc);
    #endif

    fprintf(stdout, "[GSERVER] Gracefully stopping the server\n");
    pthread_exit(NULL);
    close(socket_desc);
    exit(0);
}

static void
usage(char **argv)
{
    fprintf(stdout, "usage: %s [-h host] [-p port] | [-v]\n", argv[0]);
}

/**
 * gcc -o c_gopher -Wall -pedantic -I ../lib -lpthread -lgcc_s \
 * -D _GNU_SOURCE -DDEBUG -DVERSION=1.0 server.c ../lib/\*.c
 */

int
main(int argc , char *argv[])
{

    /*chdir(GROOT);
    if (chroot(GROOT) != 0) {
       perror("Error chroot");
    exit(-1);
    }*/

    int long_index =0;
    int opt;
    char *host = DEFAULT_HOST;
    int port = DEFAULT_PORT;

    while ((opt = getopt_long(argc, argv,"h:p:s:v",
                   long_options, &long_index )) != -1) {
        switch(opt) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'v':
                printf("%s %.1f", argv[0], VERSION);
                return 0;
            default:
                usage(argv);
                exit(EXIT_FAILURE);
        }
    }

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_flags =  SA_SIGINFO;
    act.sa_handler = &handle_quit;

    sigaction(SIGINT, &act, NULL);

    start_server(host, port);

    return 0;
}

int
close_sock(int fd, int linger) {
    if (linger) {
        struct linger opts;
        opts.l_onoff = 1;
        opts.l_linger = 30;
        if( setsockopt(fd, SOL_SOCKET, SO_LINGER, &opts, sizeof(opts)) < 0 )
            perror(NULL);
    }

    if( close( fd ) < 0 )
        return 0;
    return 1;
}

/*
 * This will handle connection for each client
 * */
void *
connection_handler(void *args) {

    /* Retrieve thread params */
    struct conn_handler_arg *th_arg;

    th_arg = (struct conn_handler_arg*) args;

    //Get the socket descriptor
    int sock = th_arg->sock;

    int read_size;
    char request[BUFFER_SIZE];
    char *req_path;

    //Receive a message from client
    while( (read_size = recv(sock , request , BUFFER_SIZE , 0)) > 0 ) {
        if(read_size <= 0) {
            request[read_size] = '\0';
            fprintf(stdout, "RECV %s", request);
            break;
        }

        req_path = build_path(request);
        #ifdef DEBUG
        fprintf(stdout, "Client is looking for: %s with len %ld\n", request, strlen(request));
        fprintf(stdout, "Retrieving data at path: %s with len %ld\n", req_path, strlen(req_path));
        fprintf(stdout, "GROOT: %s with len %ld\n", GROOT, strlen(GROOT));
        #endif

        if(isRoot(req_path) == 0) {
            asprintf(&req_path, "%s/%s", req_path, "gophermap");
            g_send_resource(sock, req_path);
        }
        else if(exists(req_path, ISDIR) == TRUE) {
            g_send_dir(sock, req_path);
        } else {
            g_send_resource(sock, req_path);
        }
        g_send(sock, ".");

        //clear the message buffer
        memset(request, 0, BUFFER_SIZE);
        close_sock(sock, 1);
    }

    pthread_exit(NULL);
    return 0;
}

