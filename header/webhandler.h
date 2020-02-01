#ifndef _WEB_HANDLER_
#define _WEB_HANDLER_
#include <libwebsockets.h>
#include "common.h"

char * get_mime_type(const char * );
int send_static_page(const char * , struct lws * );
int handle_web_request(const char * , struct lws * , bool * , struct request *);

#endif
