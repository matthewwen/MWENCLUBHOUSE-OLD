#ifndef _API_HEADER_
#define _API_HEADER_
#include <libwebsockets.h>
#include "common.h"

int handle_api_request(const char *, struct lws *, bool *, struct request *);

#endif
