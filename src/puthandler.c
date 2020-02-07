#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "puthandler.h"

int handle_put_request(const char * url, struct lws * wsi, void ** a_request) {
	//struct request * r = *a_request;
	if (strcmp(url, "/create") == 0) {
	}
	return 0;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
