/***
 *
 *  GOPHERD SERVER: A gopher server implementation
 *  that follows rfc1436
 *  Copyright (c) 2019, fmount <fmount@inventati.org>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "gophermap.h"

#include "defaults.h"

g_elem *
new_element(char *line, char *host, unsigned int port)
{
    const char del[2] = "\t";
    char *token;

    #ifdef DEBUG
    fprintf(stdout, "%s\n", line);
    fprintf(stdout, "Got type: %c\n", line[0]);
    #endif

    /**
     * Tokenize the line and create the item
     * accordingly
     */

    if(strlen(line) < MIN_LEN)
        return NULL;

    g_elem *item = (g_elem*) malloc(sizeof(g_elem));
    item->type = (char) line[0];
    line++;
    token = strtok((line), del);
    int step = 0;
    // We can improve this part .. I don't like too much ..
    item->host = NULL;
    item->port = -1;
    while (token != NULL) {
        switch(step) {
            case DESCRIPTION:
                fprintf(stdout, "Got Description: %s\n", token);
                item->description = token;
                break;
            case SELECTOR:
                fprintf(stdout, "Got Selector: %s\n", token);
                item->selector = token;
                break;
            case HOST:
                fprintf(stdout, "Got Host: %s\n", token);
                item->host = token;
                break;
            case PORT:
                fprintf(stdout, "Got Port: %s\n", token);
                item->port = atoi(token);
                break;
            default:
                break;
        }
        step++;
        token = strtok(NULL, del);
    }
    if(item->host == NULL || item->port == -1) {
        item->host = host;
        item->port = port;
    }
    return item;
}

int
isvalid_type(char *c)
{
    return ( (
		( c[0] >= '0' && c[0] <= '9' ) ||
		( c[0] >= 'A' && c[0] <= 'Z' ) ||
		( c[0] >= 'a' && c[0] <= 'z' ) ) &&
        ( strchr(c, '\t') || c[0] == 'i' ) );
}

/**
 * Parse the gphermap passed to the parser:
 * 1. analyze the line structure (and check type)
 * 2. line by line, create a list of items that represent the parsed map
 * 3. if rhost/rport are null (not valid), add the default host + port to the struct
 */
void
parse_gophermap(const char *fpath, g_elem **elements, char *rhost, unsigned int rport) {

    if(rhost == NULL || rport == 0)
        return;

    char *line = NULL;
    size_t size = 0;
    g_elem *cur = NULL;
    FILE *f = fopen(fpath, "r");
    if(!f) return;

    for(unsigned int i = 0; getline(&line, &size, f) != -1; i++ ){

        if (isvalid_type(line)) {
            cur = new_element(line, rhost, rport);
            #ifdef DEBUG
            fprintf(stdout, "[TYPE]: %c\n[DESC]: %s\n[SELEC]: %s\n[HOST]: %s\n[PORT]: %d\n",
                    cur->type, cur->description, cur->selector, cur->host, cur->port);
            #endif
        } else {
            /* it's just an INFO line, let's print it .. */
            fprintf(stdout, "%s", line);
        }
    }
    free(line);
    free(cur);
    fclose(f);
}

void
g_send(int sock, char *m)
{
    FILE *fd = fdopen(sock, "wa");
    if(m == NULL || strlen(m) <= 2)
        fprintf(fd, "%s\n", CRLF);
    else
        fprintf(fd, "i%s%s\n", m, CRLF);
    fflush(fd);
}

void
g_elem_send(int sock, g_elem *e)
{
    FILE *fd = fdopen(sock, "wa");
    if (e == NULL)
        return;
    fprintf(fd, "%c%s\t%s\t%s\t%d%s\n",
            e->type,
            e->description,
            e->selector,
            e->host,
            e->port,
            CRLF);
    fflush(fd);
}

void
g_send_resource(int sock, char * path)
{
    FILE *f = fopen(path, "r");
    FILE *fd = fdopen(sock, "wa");

    if(f == NULL)
        return;

    fseek(f, 0L, SEEK_END);
    long len = ftell(f);
    rewind(f);

    char * line = (char *) malloc(len * sizeof(char));
    if (fread(line, len, 1, f) == 1) {
        //send(sock, line, len, 0);
        fprintf(fd, "%s\n", line);
    }
    fflush(fd);
    free(line);
    fclose(f);
    fclose(fd);
}

void
g_send_dir(int sock, char * path)
{
    FILE *fd = fdopen(sock, "wa");
    DIR *d = opendir(path);
    struct dirent * entry = NULL;

    if(d == NULL) {
        fprintf(stderr, "Error listing dirs\n");
        return;
    }

    while((entry = readdir(d)) != NULL ) {
        if(entry->d_name[0] != '.')
            fprintf(fd, "%s\n", (entry)->d_name);
    }
    fflush(fd);
}

/**
 * A simple main to help testing in a standalone way
 * the gophermap resources parsed and validated
 *
 *  gcc -Wall -pedantic -o gophermap gophermap.c
 *
 */

/*int
main(int argc, char **argv) {

    fprintf(stdout, "A GOPHER PARSER AND VALIDATOR\n");

    parse_gophermap("../example/gophermap", NULL, "127.0.0.1", 70);

    return 0;
}*/
