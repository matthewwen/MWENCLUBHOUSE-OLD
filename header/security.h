#ifndef _SECURITY_H_
#define _SECURITY_H_

#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include "json.h"

typedef struct{
    int month;
    int day;
    int year;
    int min;
    int hour;
    int sec;
}mwentime_t;

mwentime_t get_time_from_buffer(const char * buffer);
long convert_time(mwentime_t t); // get time from january 1 of 2019
mwentime_t get_time();
void mwendate_tostring(mwentime_t t, char outputBuffer[16]);
void sha256_string(char * buffer, char outputBuffer[65]);
jobject * admin_auth(struct lws *);

#endif
