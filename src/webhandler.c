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

#ifndef TESTDEPLOYMENT
const char SOURCE_PATH[] = "/home/mwen/server";
#else
const char SOURCE_PATH[] = ".";
#endif
int pageview_handler(struct lws * wsi, bool * found, struct request * r);
char * get_mime_type(const char * url);

int handle_gweb_request(const char * url, struct lws * wsi, bool * found, struct request * r) {
	int n = 0;
	if (*found == false) {
		char www[] = "/www/";
		char other[] = "/www/other/";
		if (strncmp(other, url, sizeof(other) - 1) == 0) {
			// block html files, not css or javascript files
			char * mime = get_mime_type(url);
			if (strcmp(mime, "text/html") != 0) {
				r->response = FILE_REQUEST;
				n = send_static_page(url, wsi, r);
				*found = n == 0;
			}
		}
		else if (strncmp(www, url, sizeof(www) - sizeof(*www)) == 0) {
			r->response = FILE_REQUEST;
			n = send_static_page(url, wsi, r);
			*found = n == 0;
		}
		else if (strcmp("/createpkg", url) == 0) {
			r->response = FILE_REQUEST;
			n = send_static_page("/www/html/create.html", wsi, r);
			*found = n == 0;
		}
		else if (strcmp("/pageview", url) == 0) {
			n = pageview_handler(wsi, found, r);
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

int pageview_handler(struct lws * wsi, bool * found, struct request * r) {
	int n = 0;

	// get url arguments
	char * url_arg = get_header_data(wsi, WSI_TOKEN_HTTP_URI_ARGS);

	char * page_view = NULL;
	if (url_arg != NULL) {
		page_view = get_url_arg(url_arg, "name=");
		FREE(url_arg);
	}

	// check the database to make the page visibile
	bool is_valid = get_is_visible(page_view);
	r->response = FILE_REQUEST;
	if (is_valid == false) {
		jobject * robj = admin_auth(wsi);
		if (robj == NULL) {
			FREE(page_view);
			*found = true;
			return send_static_page("/www/html/template/empty.html", wsi, r);
		}
		jobject * temp = get_jobject("canEdit", robj);
		is_valid = temp != NULL && temp->type == CON && temp->data.cond;
		free_json(&robj);
	}

	char * new_url = NULL;
	const char * parent = "/www/other/";
	const char * index  = "/index.html";
	if (page_view != NULL && is_valid) {
		int num_char  = strlen(parent) + strlen(index) + strlen(page_view) + 1;
		new_url = malloc(num_char * sizeof(*new_url));
	}

	r->response = FILE_REQUEST;
	if ((is_valid = (new_url != NULL))) {
		sprintf(new_url, "%s%s", parent, page_view);
		DIR * dir = opendir(new_url + 1);
		is_valid = dir != NULL;
		if (dir) {closedir(dir);}
	}
	FREE(page_view);

	if (is_valid) {
		strcat(new_url, index);
		n = send_static_page(new_url, wsi, r);
	}
	else {
		n = send_static_page("/www/html/404.html", wsi, r);
	}

	FREE(new_url)
	*found = true;
	return n;
}

char * get_mime_type(const char * url) {
	char * end  = strrchr(url, '.');
	if (end == NULL) {
		return NULL;
	}
	else if (strcmp(end, ".js"  ) == 0) {
		return "application/javascript";
	}
	else if (strcmp(end, ".mjs" ) == 0) {
		return "text/javascript";
	}
	else if (strcmp(end, ".css" ) == 0 || strcmp(end, ".scss") == 0) {
		return "text/css";
	}
	else if (strcmp(end, ".html") == 0) {
		return "text/html";
	}
	else if (strcmp(end, ".map" ) == 0) {
		return "application/octet-stream";
	}
	else if (strcmp(end, ".jpg") == 0) {
		return "image/jpg";
	}
	else if (strcmp(end, ".png") == 0) {
		return "image/png";
	}
	else if (strcmp(end, ".pdf") == 0) {
		return "application/pdf";
	}
	else if (strcmp(end, ".webmanifest") == 0) {
		return "application/manifest+json";
	}
	else if (strcmp(end, ".txt") == 0) {
		return "plain/text";
	}
	return "plain/text";
}

int send_static_page(const char * url, struct lws * wsi, struct request * r) {
	char * mime = get_mime_type(url);
	r->mime = mime;
	char resource_path[PATH_MAX];
	memset(resource_path, 0, sizeof(resource_path));
	sprintf(resource_path, "%s%s", SOURCE_PATH, url);

	int n = access(resource_path, F_OK);
	if ( (mime != NULL) &&  (n != -1) ) {
		lws_serve_http_file(wsi, resource_path, mime, NULL, 0);
	}
	return n != -1 ? 0: -1;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
