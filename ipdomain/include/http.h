#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <linux/limits.h>

typedef int8_t protocol_t;
enum {
    GET,
    PUT,
    POST,
};

typedef int8_t service_t;
enum {
    Database,
    Storage
};

typedef int8_t key_t;
enum {
    // Database key values 
    TABLE_NAME, 
    //Storage key values
};

typedef struct{
    int year;
    int8_t month;
    int8_t day;
    int8_t hour;
    int8_t min;
    int8_t sec;
}time_t;

typedef struct header{
    key_t key;
    char * value;
    struct header * next;
}header_t;

typedef struct{
    protocol_t protocol;
    char path[PATH_MAX];
    service_t  service;
    char signature[64];
    header_t * header;
    time_t time;
}request_t;
