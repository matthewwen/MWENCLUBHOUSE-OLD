#ifndef _MOREY_IOT_H
#define _MOREY_IOT_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libwebsockets.h>
#include "common.h"

// api url request
#define MOREYIOT "/moreyIOT"

void morey_handler(const char *, struct lws * wsi, bool * found, struct request * r);

#endif
