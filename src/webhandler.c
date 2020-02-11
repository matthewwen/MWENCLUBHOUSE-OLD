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

const char SOURCE_PATH[] = ".";
int pageview_handler(struct lws * wsi, bool * found, struct request * r);
char * get_mime_type(const char * url);
int send_static_page(const char * url, struct lws * wsi);

int handle_gweb_request(const char * url, struct lws * wsi, bool * found, struct request * r) {
	int n = 0;
	if (*found == false) {
		char www[] = "/www/";
		char other[] = "/www/other/";
		if (strncmp(other, url, sizeof(other) - 1) == 0) {
			// block html files, not css or javascript files
			char * mime = get_mime_type(url);
			if (strcmp(mime, "text/html") != 0) {
				*found = true;
				r->response = FILE_REQUEST;
				n = send_static_page(url, wsi);
			}
		}
		else if (strncmp(www, url, sizeof(www) - sizeof(*www)) == 0) {
			*found = true;
			r->response = FILE_REQUEST;
			n = send_static_page(url, wsi);
		}
		else if (strcmp("/createpkg", url) == 0) {
			*found = true;
			r->response = FILE_REQUEST;
			n = send_static_page("/www/html/create.html", wsi);
		}
		else if (strcmp("/pageview", url) == 0) {
			*found = true;
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

	// check the database to make the page visibile
	bool is_valid = false;
	if (is_valid == false) {
		jobject * robj = admin_auth(wsi);
		r->response = FILE_REQUEST;
		if (robj == NULL) {
			return send_static_page("/www/html/template/empty.html", wsi);
		}

		jobject * temp = get_jobject("canEdit", robj);
		is_valid = temp != NULL && temp->type == CON && temp->data.cond;
		free_json(&robj);
	}

	char * url_arg = NULL, * page_view = NULL;
	if (is_valid == true) {
		url_arg = get_header_data(wsi, WSI_TOKEN_HTTP_URI_ARGS);
	}
	if (url_arg != NULL) {
		const char * page_key = "name=";
		page_view  = strstr(url_arg, page_key);
		page_view += page_view == NULL ? 0: strlen(page_key);
	}

	char * new_url = NULL;
	const char * parent = "/www/other/";
	const char * index  = "/index.html";
	if (page_view != NULL) {
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

	if (is_valid) {
		strcat(new_url, index);
		n = send_static_page(new_url, wsi);
	}
	else {
		n = send_static_page("/www/html/404.html", wsi);
	}

	FREE(url_arg);
	FREE(new_url);
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
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */