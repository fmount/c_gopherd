/***
 *
 *  GOPHERD SERVER: A gopher server implementation
 *  that follows rfc1436
 *  Copyright (c) 2019, fmount <fmount@inventati.org>
 *
 */

#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <stdio.h>

#define TRUE 0
#define FALSE 1
#define MIN_LEN 5
#define CRLF "\r\t"
#define BUFFER_SIZE 1024

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 70
#define GROOT "/home/fmount/private/c_gopherd/example"

typedef enum {
    ISDIR,
    ISFILE,
} FS_RESOURCE;

enum item_step {
    DESCRIPTION = 0,
    SELECTOR = 1,
    HOST = 2,
    PORT = 3,
};

#endif
