#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "http.h"

request_t get_default_request() {
    request_t request = {.protocol = NPROTOCOL,
                         .service  = NSERVICE, 
                         .length   = 0,
                         .expect   = false,
                         .header   = NULL};
    request.path[0]      = '\0';
    request.signature[0] = '\0';
    return request;
}

bool parse_request(request_t * request, char ** buffer, int * len) {
    char * pos = *buffer;
    for(; *pos == ' ' && *pos != '\0'; pos = &pos[1]) {}
    request->protocol = request->protocol == NPROTOCOL && strncmp("PUT" , pos, 3) == 0 ? PUT : NPROTOCOL;
    request->protocol = request->protocol == NPROTOCOL && strncmp("GET" , pos, 3) == 0 ? GET : NPROTOCOL;
    request->protocol = request->protocol == NPROTOCOL && strncmp("POST", pos, 4) == 0 ? POST: NPROTOCOL;

    for (char * curr = pos; curr != NULL;) {
        char * npos = strchr(curr, ':');
        char * colon = npos;
        if (npos != NULL) {
            for(; *(npos - 1) != '\n' && (npos - 1) >= curr; npos = npos - 1) {}
            if (strncmp("Expect", npos, strlen("Expect")) == 0){
                request->expect = true;
            }
            else if (strncmp("Content-Length", npos, strlen("Content-Length")) == 0) {
                request->length = atoi(colon + 1);
                printf("length: %ld\n", request->length);
            }
        }
        curr = colon != NULL ? &colon[1]: NULL;
    }

    return true;
}