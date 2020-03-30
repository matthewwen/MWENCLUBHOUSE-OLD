#ifndef _API_HEADER_
#define _API_HEADER_
#include <libwebsockets.h>
#include "common.h"

int handle_gapi_request(const char *, struct lws *, bool *, struct request *);
int send_file(const char * file_path, bool * found, struct lws * wsi, struct request * r);

#endif
