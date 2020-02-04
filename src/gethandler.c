#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "gethandler.h"
#include "webhandler.h"
#include "apihandler.h"
#include "common.h"

static char UNKNOWN[] = "{\"result\": \"unknown\"}";

int handle_get_request(const char * url, struct lws * wsi, void ** a_request) {
    int n = 0;
    bool request_found = false;
    struct request * r = *a_request;
    r->type = GET;
    
    // O(n) -> n = number of requests -> TODO -> change to hash map
    if (r->response == 0) {
        n = handle_gweb_request(url, wsi, &request_found, r);
        n = handle_gapi_request(url, wsi, &request_found, r);

        // request is not found, send unknown response 
        if (request_found == false) {
            CREATE_REQUEST(r, BUFFER_REQUEST, sizeof(UNKNOWN) - 1, UNKNOWN, BUFFER_STATIC);
            assert(r->type == GET);
            /*r = (struct request) {.response = BUFFER_REQUEST, .alloc_size = sizeof(UNKNOWN) - 1, 
                                   .buff = UNKNOWN, .pos = UNKNOWN, .free_type = BUFFER_STATIC};*/
        }

        if (r->response == BUFFER_REQUEST) {
            lws_callback_on_writable(wsi);
        }
    }

    return n;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
