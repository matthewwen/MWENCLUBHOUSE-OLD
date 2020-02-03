#ifndef _PUT_HANDLER_
#define _PUT_HANDLER_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>
#include "common.h"

int handle_put_request(const char *, struct lws *, void **);

#endif
