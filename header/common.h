#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>

typedef uint8_t free_t;
enum {
    BUFFER_STATIC,
    BUFFER_MALLOC,
};

typedef uint8_t request_t;
enum {
    FILE_REQUEST,
    BUFFER_REQUEST,
};

struct request{
    request_t response;
    size_t    alloc_size;
    char *    buff; 
    char *    pos; 
    free_t    free_type;
};

#endif