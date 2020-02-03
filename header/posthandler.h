#ifndef _POST_HANDLER_
#define _POST_HANDLER_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>
#include "common.h"

int handle_post_request(const char *, struct lws *, void **);

#endif
