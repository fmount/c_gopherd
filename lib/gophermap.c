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
    while (token != NULL) {
        //fprintf(stdout, "Got Token: %s\n", token);
        switch(step) {
            case 0:
                fprintf(stdout, "Got Description: %s\n", token);
                item->description = token;
                break;
            case 1:
                fprintf(stdout, "Got Selector: %s\n", token);
                item->selector = token;
                break;
            case 2:
                fprintf(stdout, "Got Host: %s\n", token);
                item->host = token;
                break;
            case 3:
                fprintf(stdout, "Got Port: %s\n", token);
                item->port = atoi(token);
                break;
            default:
                break;
        }
        step++;
        token = strtok(NULL, del);
    }
    if(item->host == NULL || item->port == NULL) {
        item->host = host;
        item->port = port;
    }
    return NULL;
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

    char *line = NULL;
    size_t size = 0;

    FILE *f = fopen(fpath, "r");
    if(!f) return;

    for(unsigned int i = 0; getline(&line, &size, f) != -1; i++ ){

        if (isvalid_type(line)) {
            new_element(line, rhost, rport);
        } else {
            /* it's just an INFO line */
            fprintf(stdout, "%s", line);
        }
    }
}

/**
 * A simple main to help testing in a standalone way
 * the gophermap resources parsed and validated
 *
 *  gcc -Wall -pedantic -o gophermap gophermap.c
 *
 */

int
main(int argc, char **argv) {

    fprintf(stdout, "A GOPHER PARSER AND VALIDATOR\n");

    parse_gophermap("../example/gophermap", NULL, NULL, 0);

    return 0;
}
