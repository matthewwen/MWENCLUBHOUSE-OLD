#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "security.h"

mwentime_t get_time_from_buffer(const char * buffer) {
    mwentime_t t;
    memset(&t, 0, sizeof(t));
    return t;
}

long convert_time(mwentime_t t) {
    return 0;
}

mwentime_t get_time() {
    mwentime_t t;
    memset(&t, 0, sizeof(t));
    time_t rawtime;
    time(&rawtime);
    struct tm * info = gmtime(&rawtime);
    t.year  = info->tm_year;
    t.month = info->tm_mon;
    t.day   = info->tm_mday;
    t.hour  = info->tm_hour;
    t.min   = info->tm_sec;
    return t;
}

/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
