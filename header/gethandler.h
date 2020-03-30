#ifndef _GET_HANDLER_
#define _GET_HANDLER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>
#include "common.h"

int handle_get_request(const char *, struct lws *, void **);

#endif
