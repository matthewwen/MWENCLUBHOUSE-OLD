#include <stdio.h>
#include <stdlib.h>
#include "posthandler.h"
#include "common.h"

int handle_post_request(const char * url, struct lws * wsi, void ** a_request) {
	struct request * r = *a_request;
	r->type = POST;
	return 0;
}

/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
