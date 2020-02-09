#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <linux/limits.h>
#include "webhandler.h"

const char SOURCE_PATH[] = ".";

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
	return NULL;
}

int send_static_page(const char * url, struct lws * wsi) {
	char * mime = get_mime_type(url);
	char resource_path[PATH_MAX];
	memset(resource_path, 0, sizeof(resource_path));
	sprintf(resource_path, "%s%s", SOURCE_PATH, url);

	int n = access(resource_path, F_OK);
	if ( (mime != NULL) &&  (n != -1) ) {
		lws_serve_http_file(wsi, resource_path, mime, NULL, 0);
	}
	return n != -1 ? 0: -1;
}

int handle_gweb_request(const char * url, struct lws * wsi, bool * found, struct request * r) {
	int n = 0;
	if (*found == false) {
		char www[] = "/www/";
		if (strncmp(www, url, sizeof(www) - sizeof(*www)) == 0) {
			*found = true;
			r->response = FILE_REQUEST;
			n = send_static_page(url, wsi);
		}
		else if (strcmp("/createpkg", url) == 0) {
			*found = true;
			r->response = FILE_REQUEST;
			n = send_static_page("/www/html/create.html", wsi);
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
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
