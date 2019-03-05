/***
 *
 *  GOPHERD SERVER: A gopher server implementation
 *  that follows rfc1436
 *  Copyright (c) 2019, fmount <fmount@inventati.org>
 *
 */


void handlerequest(int connfd);
void serve(int sockd);
int start_server();
int stop_server();
