/***
 *
 *  GOPHERD SERVER: A gopher server implementation
 *  that follows rfc1436
 *  Copyright (c) 2019, fmount <fmount@inventati.org>
 *
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "defaults.h"


/* called this function
 * when a syscall fails
 *
 * */
void error(char* msg) {
    perror(msg);
    exit(1);
}

char *
ext(char* entry)
{
    char *dot = strrchr(entry, '.');
    if(!dot || dot == entry)
        return "";
    return dot + 1;
}

/*
 * wrapper for write() with 
 * error handling 
 *
 * */
void
write_data(int sockfd, const char* data, int len) {
    if (write(sockfd, data, len) < 0)
        error("[BUF] Error writing sock");
}

int
exists(const char *resource, int mode) {
    struct stat sb;
    int exist;

    /*
     * mode = ISDIR|ISFILE;
     * MODE COULD BE ISDIR/ISFILE
     */

    switch(mode) {
        case ISDIR:
            exist = (stat(resource, &sb) == 0 && \
             S_ISDIR(sb.st_mode)) ? (TRUE) : (FALSE);
            break;
        case ISFILE:
             exist = (stat(resource, &sb) == 0 && \
              S_ISREG(sb.st_mode)) ? (TRUE) : (FALSE);
             break;
        default:
             exist = 1;
             break;
    }
    return exist;
}


/*
 * write a file to socket 
 * TODO: I need to optimize this 
 * function
 *
 * */
void printfile(int sockfd, const char* path) {

    /* open file */
    FILE* f = fopen(path, "rb");

    if(f == 0) {
        /* error reading file */
        write_data(sockfd, ERROR_STRING, strlen(ERROR_STRING));
    }
    else {
        /* line buffer */
        char* buf = NULL;

        /* line length */
        size_t len = 0;

        /* bytes read */
        ssize_t read = 0;

        while((read = getline(&buf, &len, f)) != -1) {
            /* allocate space for adjusted line */
            char* line = calloc(read + 2, sizeof(char));

            /* replace \n with \r\n */
            strlcpy(line, buf, read);
            strlcat(line, "\r\n\0", read + 2);

            /* write line to socket */
            write_data(sockfd, line, strlen(line));

            /* free buffers */
            free(buf);
            free(line);
        }

        /* close file */
        fclose(f);
    }
}
