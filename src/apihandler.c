#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "apihandler.h"
#include "json.h"
#include "security.h"

char * get_header_data(struct lws * wsi, enum lws_token_indexes h) {
    char * str = NULL;
    int count = lws_hdr_total_length(wsi, h) + 1;
    if (count != 0) {
        str = malloc(count * sizeof(*str));
        memset(str, 0, count * sizeof(*str));
        lws_hdr_copy(wsi, str, count, h);
    }
    return str;
}

char * get_custom_header_data(struct lws * wsi, const char * token) {
    char * str = NULL;
    int token_len = strlen(token);
    int count = lws_hdr_custom_length(wsi, token, token_len) + 1;
    if (count != 0) {
        str = malloc(count * sizeof(*str));
        memset(str, 0, count * sizeof(*str));
        lws_hdr_custom_copy(wsi, str, count, token, token_len);
    }
    return str;
}

int handle_gapi_request(const char * url, struct lws * wsi, bool * found, struct request * r) {
    int n = 0;
    if (*found == false) {
        if (strcmp("/webedit", url) == 0) {
            char * token = get_header_data(wsi, WSI_TOKEN_HTTP_AUTHORIZATION);
            printf("authorization: %s\n", token);
            free(token);
            
            char * date = get_custom_header_data(wsi, "mwen-date:");
            printf("date: %s\n", date);
            if (date != NULL) {
               free(date);
            }

            char * response = "{\"authorization\": \"hithere\"}";
            size_t alloc_size = strlen(response) * sizeof(*response);

            mwentime_t t = get_time();
            printf("month: %d, ", t.month);

            CREATE_REQUEST(r, BUFFER_REQUEST, alloc_size, response, BUFFER_STATIC);
            *found = true;
        }
    }
    return n;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
