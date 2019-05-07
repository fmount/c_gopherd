#include<stdio.h>

/**
 * Define some costants to
 * be used during the design/implementation
 *
 * TODO: These will be replaced by a config file
 *
 */

#define DEFAULT_PORT 70
#define ERROR_STRING "3Invalid input\tfake\t(NULL) 0"
#define TRUE 0
#define FALSE 1
#define MIN_LEN 5

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
