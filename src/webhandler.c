#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <linux/limits.h>
#include <dirent.h>
#include <errno.h>
#include "webhandler.h"
#include "json.h"
#include "common.h"
#include "security.h"
#include "webdatabase.h"
#include "apihandler.h"

#ifndef TESTDEPLOYMENT
const char SOURCE_PATH[] = "/home/mwen/server";
static const char * PAGEVIEW_PARENT = "/home/mwen/other/";
#define PAGEVIEW_PARENT_LEN 17
#else
const char SOURCE_PATH[] = ".";
static const char * PAGEVIEW_PARENT = "/home/mwen/Dropbox/outsideprojects/other/";
#define PAGEVIEW_PARENT_LEN 41
#endif
int pageview_handler(struct lws * wsi, const char * url, bool * found, struct request * r);
char * get_mime_type(const char * url);

int handle_gweb_request(const char * url, struct lws * wsi, bool * found, struct request * r) {
	int n = 0;
	if (*found == false) {
		char www[] = "/www/";
		if (strncmp(www, url, sizeof(www) - sizeof(*www)) == 0) {
			r->response = FILE_REQUEST;
			n = send_static_page(url, wsi, r);
			*found = n == 0;
		}
		else if (strcmp("/createpkg", url) == 0) {
			r->response = FILE_REQUEST;
			n = send_static_page("/www/html/create.html", wsi, r);
			*found = n == 0;
		}
		else if (strncmp("/pageview", url, strlen("/pageview")) == 0) {
			n = pageview_handler(wsi, url, found, r);
		}

		if (*found) {
			if (n == -1) {
				r->response = 0;
				*found = false;
				n = 0;
			}
			else {
				r->response = FILE_REQUEST;
			}
		}
	}
	return n;
}

#define PAGEVIEW_NAME_LEN 10

int pageview_handler(struct lws * wsi, const char * url, bool * found, struct request * r) {
	url += strlen("/pageview") + 1;
	int n = 0;

	// get url arguments
	char page_view[PAGEVIEW_NAME_LEN + 1];
	memset(page_view, 0, sizeof(page_view));
	if (strlen(url) >= PAGEVIEW_NAME_LEN) {
		strncpy(page_view, url, PAGEVIEW_NAME_LEN);
		url += PAGEVIEW_NAME_LEN;
	}

	// check the database to make the page visibile
	bool is_valid = get_is_visible(page_view);
	r->response = FILE_REQUEST;
	if (is_valid == false) {
		jobject * robj = admin_auth(wsi);
		if (robj == NULL) {
			*found = true;
			return send_static_page("/www/html/template/empty.html", wsi, r);
		}
		jobject * temp = get_jobject("canEdit", robj);
		is_valid = temp != NULL && temp->type == CON && temp->data.cond;
		free_json(&robj);
	}

	char new_url[PAGEVIEW_PARENT_LEN + PAGEVIEW_NAME_LEN + 1];
	memset(new_url, 0, sizeof(new_url));
	if (is_valid) {
		sprintf(new_url, "%s%s", PAGEVIEW_PARENT, page_view);
	}

	r->response = FILE_REQUEST;
	if (is_valid) {
		DIR * dir = opendir(new_url);
		is_valid = dir != NULL;
		if (dir) {closedir(dir);}
	}

	if (is_valid) {
		url += (*url == '/') ? 1: 0;
		const char * file_name = (*url == '\0') ? "index.html": url;
		size_t alloc_size = strlen(file_name) + strlen(new_url) + 2;
		char * file_path = malloc(alloc_size);
		if (file_path != NULL) {
			sprintf(file_path, "%s/%s", new_url, file_name);
			n = send_file(file_path, wsi, r);
			free(file_path);
		}
	}
	else {
		n = send_static_page("/www/html/404.html", wsi, r);
	}

	*found = true;
	return n;
}

int send_static_page(const char * url, struct lws * wsi, struct request * r) {
	char resource_path[PATH_MAX];
	memset(resource_path, 0, sizeof(resource_path));
	sprintf(resource_path, "%s%s", SOURCE_PATH, url);
	return send_file(resource_path, wsi, r);
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */