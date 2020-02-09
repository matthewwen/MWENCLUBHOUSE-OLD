#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "puthandler.h"
#include "security.h"
#include "json.h"

int handle_put_request(const char * url, struct lws * wsi, void ** a_request) {
	if (strcmp(url, "/create") == 0) {
		jobject * obj = admin_auth(wsi);
		if (obj != NULL) {
			jobject * jcanEdit = get_jobject("canEdit", obj);
			if (jcanEdit != NULL && jcanEdit->type == CON && jcanEdit->data.cond == true) {
				printf("can edit\n");
			}
			else {
				printf("cannot edit\n");
			}
			free_json(&obj);
		}
	}
	return 0;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
