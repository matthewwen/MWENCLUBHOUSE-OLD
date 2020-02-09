#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "apihandler.h"
#include "json.h"
#include "security.h"
#include "common.h"

void webedit_handler(struct lws * wsi, bool * found, struct request * r);

int handle_gapi_request(const char * url, struct lws * wsi, bool * found, struct request * r) {
	int n = 0;
	if (*found == false) {
		if (strcmp("/webedit", url) == 0) {
			webedit_handler(wsi, found, r);
		}
		else if (strcmp("/create", url) == 0) {
			jobject * robj = admin_auth(wsi);
			if (robj != NULL) {
				json_tostring(&r->buff, robj, &r->alloc_size);
				CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
				free_json(&robj);
				*found = true;
			}
		}
	}
	return n;
}

void webedit_handler(struct lws * wsi, bool * found, struct request * r) {
	jobject * robj = admin_auth(wsi);
	if (robj != NULL) {
		json_tostring(&r->buff, robj, &r->alloc_size);
		CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
		free_json(&robj);
		*found = true;
	}
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
