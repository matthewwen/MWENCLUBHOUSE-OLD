#ifndef _API_HEADER_
#define _API_HEADER_
#include <libwebsockets.h>

int handle_api_request(const char * url, struct lws * wsi, bool * found);

#endif
