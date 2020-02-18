#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "moreyIOT.h"
#include "common.h"
#include "security.h"
#include "json.h"

void light_handler(struct lws * wsi, bool * found, struct request * r);

void morey_handler(const char * url, struct lws * wsi, bool * found, struct request * r) {
    url = url + strlen(MOREYIOT);
    if (strcmp(url, "/light") == 0) {
        light_handler(wsi, found, r);
    }
}

void light_handler(struct lws * wsi, bool * found, struct request * r) {
    jobject * robj = admin_auth(wsi);
    jobject * temp = get_jobject("canEdit", robj);
    bool is_valid = temp != NULL && temp->data.cond; // change later

    char * url_arg = NULL, * servicestr = NULL;
    if (is_valid) {
        url_arg    = get_header_data(wsi, WSI_TOKEN_HTTP_URI_ARGS);
        servicestr = get_url_arg(url_arg, "service=");
    }
    if (url_arg != NULL) {free(url_arg);}

    int service = -1;
    if (servicestr != NULL) {
        service = atoi(servicestr);
    }

    char * jsonservice = "unknown";
    if (service == 1) {
        jsonservice = "aws";
	system("python3 py/pub.py");
    }
    else if (service == 2) {
        jsonservice = "azure";
    }
    else if (service == 3) {
        jsonservice = "google cloud";
    }

    if (servicestr != NULL) {
    	append_jobject(&robj, "service", TEXT_STATIC, (data_t) {.txt = jsonservice});
        free(servicestr);
    }

    json_tostring(&r->buff, robj, &r->alloc_size);
    free_json(&robj);
    if ((*found = (r->buff != NULL))) {
        CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
    }
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
