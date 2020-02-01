#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "apihandler.h"

int handle_api_request(const char * url, struct lws * wsi, bool * found, struct request * r) {
    int n = 0;
    if (*found == false) {
        if (false) {
            r->response = BUFFER_REQUEST;
        }
    }
    return n;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
