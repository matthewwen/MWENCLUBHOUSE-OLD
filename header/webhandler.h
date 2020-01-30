#ifndef _WEB_HANDLER_
#define _WEB_HANDLER_
#include <libwebsockets.h>

char * get_mime_type(const char * url);
int send_static_page(const char * url, struct lws * wsi);
int handle_web_request(const char * url, struct lws * wsi, bool * found);

#endif
