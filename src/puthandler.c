#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "puthandler.h"
#include "security.h"
#include "json.h"

int handle_put_request(const char * url, struct lws * wsi, void ** a_request) {
	if (strcmp(url, "/puturl") == 0) {
	}
	return 0;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
