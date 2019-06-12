/***
 *
 *  GOPHERD SERVER: A gopher server implementation
 *  that follows rfc1436
 *  Copyright (c) 2019, fmount <fmount@inventati.org>
 *
 */

#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "defaults.h"


/*
 * Call this function
 * when a syscall fails
 *
 * */
void error(char* msg) {
    perror(msg);
    exit(1);
}

/*
 * Get the file extension
 */

char *
ext(char* entry)
{
    char *dot = strrchr(entry, '.');
    if(!dot || dot == entry)
        return "";
    return dot + 1;
}

/**
 * Build the path of the resource
 * that should be retrieved from
 * the srv directory of the server
 */

char *
build_path(char *request)
{
    char *result;

    if(request[0] == '\0' || strcmp(request, CRLF) == 0) {
        fprintf(stdout, "request is empty");
        return (char *)GROOT;
    }
    asprintf(&result, "%s/%s", GROOT, request + (request[0] == '/' ? 1 : 0));
    result[strlen(result) -2] = '\0';
    #ifdef DEBUG
    fprintf(stdout, "Client Request is: %s\n", request);
    fprintf(stdout, "[BUILD] PATH is %s with len %d\n", result, strlen(result));
    #endif
    return result;
}

/*
 * Check if the resource exists: according
 * to the `mode` param, check if is a DIR
 * or a FILE using lstat,
 */

int
exists(const char *resource, int mode) {
    struct stat sb;
    int exist;

    if(resource == NULL)
        return FALSE;
    /*
     * mode = ISDIR|ISFILE;
     */
    switch(mode) {
        case ISDIR:
            exist = (lstat((char*)resource, &sb) == 0 && \
             S_ISDIR(sb.st_mode)) ? (TRUE) : (FALSE);
            break;
        case ISFILE:
             exist = (lstat((char*)resource, &sb) == 0 && \
              S_ISREG(sb.st_mode)) ? (TRUE) : (FALSE);
             break;
        default:
             exist = 1;
             break;
    }
    return exist;
}

/**
 * Check if the requested path
 * is the GROOT: if true, we can
 * return the gophermap
 */

int
isRoot(char * path)
{
    struct stat sb;

    if((lstat((char*)path, &sb) != 0 || \
                ! S_ISDIR(sb.st_mode)))
            return FALSE;

    DIR *d = opendir(path);
    struct dirent * entry = NULL;
    while((entry = readdir(d)) != NULL) {
        if(strncmp(entry->d_name, "gophermap", strlen("gophermap")) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

#endif

