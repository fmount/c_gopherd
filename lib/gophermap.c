/***
 *
 *  GOPHERD SERVER: A gopher server implementation
 *  that follows rfc1436
 *  Copyright (c) 2019, fmount <fmount@inventati.org>
 *
 */

#include <stdio.h>
#include "gopheritems.h"



void
parse_gophermap(const char *fpath, g_elem **elements, const char *rhost, unsigned int rport) {

    char *line = NULL;
    size_t size = 0;

    FILE *f = fopen(fpath, "r");
    if(!f) return;

    for(unsigned int i = 0; getline(&line, &size, f) != -1; i++ ){

        fprintf(stdout, "[LINE]: %s", line);
        /*
         * if ( validate_item(line) ){

            elements[i] = new_element( line[0], line+1, rhost, rport );
        } else {
            elements[i] = process_line( 'i', line, rhost, rport );
        }

        */
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
