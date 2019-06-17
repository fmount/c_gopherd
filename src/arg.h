/***
 *
 *  GOPHERD SERVER: A gopher server implementation
 *  that follows rfc1436
 *  Copyright (c) 2019, fmount <fmount@inventati.org>
 *
 */

#include <getopt.h>

static struct option long_options[] = {
    { "host", required_argument, 0, 'h'},
    { "port", required_argument, 0, 'p'},
    { "serve",required_argument, 0, 's'},
    {0, 0, 0, 0 }
};

struct conn_handler_arg {
    int sock;
    char *srv;
};
