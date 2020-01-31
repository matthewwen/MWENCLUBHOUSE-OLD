#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "gethandler.h"
#include "webhandler.h"
#include "apihandler.h"

int handle_get_request(const char * url, struct lws * wsi) {
    int n = 0;
    bool request_found = false;
    
    // O(n) -> n = number of requests -> TODO -> change to hash map
    n = handle_web_request(url, wsi, &request_found);
    n = handle_api_request(url, wsi, &request_found);

    // request is not found, send unknown response 
    if (request_found == false) {
        char unknown[] = "unknown";
        printf("write unknown response\n");
        lws_write(wsi, (void * ) unknown, (strlen(unknown) * sizeof(*unknown)), LWS_WRITE_HTTP);
    }

    return n;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
