/***
 *
 *  GOPHERD SERVER: A gopher server implementation
 *  that follows rfc1436
 *  Copyright (c) 2019, fmount <fmount@inventati.org>
 *
 */

#ifndef SERVER_H
#define SERVER_H

void *handlerequest(void *connfd);
void serve(int sockd);
int start_server(char *addr, int port);
int stop_server(int sockd);

#endif /* !SERVER_H */
