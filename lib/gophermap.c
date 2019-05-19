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
    fprintf(fd, "%c%s\t%s\t%s\t%d%s",
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
        fprintf(fd, "%s%s", line, CRLF);
    }
    fflush(fd);
    free(line);
    fclose(f);
}

char *
suffix(char * entry)
{
    char *dot = strrchr(entry, '.');
    if(!dot || dot == entry)
        return "";
    return dot + 1;
}

char
get_type(struct dirent *entry, char * suffix)
{
    char type;
    switch(entry->d_type) {
        case DT_REG:
            if (strncmp(suffix, "zip", 3) == 0)
                type = (char) G_ARCHIVE;
            else if (strncmp(suffix, "jpg", 3) == 0)
                type = (char) G_IMAGE;
            else if (strncmp(suffix, "png", 3) == 0)
                type = (char) G_PNG;
            else if (strncmp(suffix, "pdf", 3) == 0)
                type = (char) G_PDF;
            else if (strncmp(suffix, "gif", 3) == 0)
                type = (char) G_GIF;
            else
                type = (char) G_FILE;
            break;
        case DT_DIR:
            type = (char) G_DIR;
            break;
        default:
            type = (char) G_ERROR;
            break;
    }
    #ifdef DEBUG
    fprintf(stdout, "[DEBUG] Type detected: %c\n", type);
    #endif
    return type;
}

void
g_send_dir(int sock, char * path)
{
    FILE *fd = fdopen(sock, "wa");
    DIR *d = opendir(path);
    char *e;
    char *selector;
    char type;
    struct dirent * entry = NULL;

    if(d == NULL) {
        fprintf(stderr, "Error listing dirs\n");
        return;
    }

    while((entry = readdir(d)) != NULL ) {
        fprintf(stdout, "START WITH %c\n", entry->d_name[0]);
        if(entry->d_name[0] != '.') {
            selector = malloc( sizeof(char)*(strlen(path)+strlen(entry->d_name)+2) );
            e = suffix(entry->d_name);
            type = get_type(entry, e);
            strncpy(selector, path, strlen(path));
            /* Do I need to add a trailing '/' ? */
            g_elem * gopher_item = (g_elem*)malloc(sizeof(g_elem));
            gopher_item->type = type;
            gopher_item->description = entry->d_name;
            gopher_item->selector = selector;

            gopher_item->host = "test";
            gopher_item->port = 70;

            g_elem_send(sock, gopher_item);
            g_send(sock, NULL);
            free(gopher_item);
            free(selector);
        }
    }
    g_send(sock, NULL);
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
