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
int admin_pageview_handler(struct lws * wsi, bool * found, struct request * r);
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
		else if (strcmp("/pageview", url) == 0 || strcmp("/pageview/", url) == 0) {
			admin_pageview_handler(wsi, found, r);
		}
		else if (strncmp("/pageview/", url, strlen("/pageview/")) == 0) {
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

int admin_pageview_handler(struct lws * wsi, bool * found, struct request * r) {
	r->response = FILE_REQUEST;
	int n = send_static_page("/www/html/pageview.html", wsi, r);
	*found = n == 0;
	return n;
}

int pageview_handler(struct lws * wsi, const char * url, bool * found, struct request * r) {
	url += strlen("/pageview") + 1;
	int n = 0;

	// get url arguments
	char page_view[PAGEVIEW_NAME_LEN + 1];
	memset(page_view, 0, sizeof(page_view));
	bool is_valid = *url != '\0' && strlen(url) >= PAGEVIEW_NAME_LEN;
	if (is_valid) {
		strncpy(page_view, url, PAGEVIEW_NAME_LEN);
		url += PAGEVIEW_NAME_LEN;
	}

	char new_url[PAGEVIEW_PARENT_LEN + PAGEVIEW_NAME_LEN + 1];
	memset(new_url, 0, sizeof(new_url));
	if (is_valid) {
		sprintf(new_url, "%s%s", PAGEVIEW_PARENT, page_view);
		r->response = FILE_REQUEST;
		DIR * dir = opendir(new_url);
		is_valid = dir != NULL;
		if (dir) {closedir(dir);}
	}

	// check the database to make the page visibile
	if (is_valid) {
		is_valid = get_is_visible(page_view);
		r->response = FILE_REQUEST;
	}

	if (is_valid == false) {
		n = send_static_page("/www/html/404.html", wsi, r);
	}
	else {
		url += (*url == '/') ? 1: 0;
		const char * file_name = (*url == '\0') ? "index.html": url;
		size_t alloc_size = strlen(file_name) + strlen(new_url) + 2;
		char * file_path = malloc(alloc_size);
		memset(file_path, 0, alloc_size);
		if (file_path != NULL) {
			sprintf(file_path, "%s/%s", new_url, file_name);
			n = send_file(file_path, wsi, r);
			free(file_path);
		}
	}

	*found = n == 0;
	return n;
}

int send_static_page(const char * url, struct lws * wsi, struct request * r) {
	char resource_path[PATH_MAX];
	memset(resource_path, 0, sizeof(resource_path));
	sprintf(resource_path, "%s%s", SOURCE_PATH, url);
	return send_file(resource_path, wsi, r);
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */