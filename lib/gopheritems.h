/***
 *
 *  GOPHERD SERVER: A gopher server implementation
 *  that follows rfc1436
 *  Copyright (c) 2019, fmount <fmount@inventati.org>
 *
 *  ----------------------------
 *  |  type   |Description      |
 *  |---------|-----------------|
 *  |0        |text             |
 *  |1        |gopher menu      |
 *  |5        |zip file         |
 *  |7        |search server    |
 *  |9        |generic binary   |
 *  |I        |generic image    |
 *  |g        |gif image        |
 *  |s        |sound/audio file |
 *  |h        |html             |
 *  ----------------------------
 */

enum item_types {
    G_FILE='0',
    G_DIR='1',
    G_ERROR='3',
    G_ARCHIVE='5',
    G_BINARY='9',
    G_IMAGE='I',
    G_INFO='i',
    G_GIF='g',
    G_PNG='p',
    G_PDF='d',
    G_ITEM=0
};

typedef struct element {
    char *type;
    char *selector;
    char *content;
    // host + port to make a call and retrieve the content ?
    int port;
    char *host;
} g_elem;


/**
 * Prototyping some needed functions to parse gophermap
 */

g_elem *new_element(char *line, char *host, unsigned int port);
void parse_gophermap(const char * fpath, g_elem ** elements, const char * rhost, unsigned int rport );
int validate_item(const char *rline);


/**
 *  SOME NOTES TO VALIDATE LINES:
 *
 *  According to protocol, we need to check if a given line is valid:
 *
 *  1. Does begin with a valid ASCII character 
 *  2. Has at least one TAB
 *  3. Is at least 5 chars long
 *  4. OR it begins with 'i'
 *
 *  Each line ends with a <CR><LF> (aka \r\t) sequence
 *
 *  Example:
 *
 *  0Gopher
 *  Introduction<Tab>intro<Tab>gopher.someserver.org<Tab>70
 *
 */
