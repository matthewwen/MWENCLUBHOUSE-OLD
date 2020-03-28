#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include "apihandler.h"
#include "json.h"
#include "security.h"
#include "common.h"
#include "msqlite.h"
#include "webdatabase.h"

#include "moreyIOT.h"
#include "todo.h"

static const char * TEMPLATE_PARENT = "www/other/";
static const char * TEMPLATE_INDEX  = "/index.html";
static const char * TEMPLATE_MAINJS = "/main.js";
static const char * TEMPLATE_CSS    = "/main.css";

#define MAX_FILE_LENGTH  11
#define ID_LENGTH 10

void webedit_handler(struct lws * wsi, bool * found, struct request * r);
void createproj_handler(struct lws * wsi, bool * found, struct request * r);
void getproj_handler(struct lws * wsi, bool * found, struct request * r);

void format_href(char * href) {
	for (int i = 0; href[i] != '\0'; i++) {
		if (href[i] == '-') {
			href[i] = '_';
		}
	}

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

int send_file(const char * file_path, struct lws * wsi, struct request * r) {
	char * mime = get_mime_type(file_path);
	r->mime = mime;

	int n = access(file_path, F_OK);
	if ( (mime != NULL) &&  (n != -1) ) {
		lws_serve_http_file(wsi, file_path, mime, NULL, 0);
	}
	return n != -1 ? 0: -1;
}

int handle_gapi_request(const char * url, struct lws * wsi, bool * found, struct request * r) {
	int n = 0;
	if (*found == false) {
		if (strcmp("/webedit", url) == 0) {
			webedit_handler(wsi, found, r);
		}
		else if (strcmp("/mwenCreatePROJ", url) == 0) {
			createproj_handler(wsi, found, r);
		}
		else if (strcmp("/mwenGetPROJ", url) == 0) {
			getproj_handler(wsi, found, r);
		}
		else if (strncmp(MOREYIOT, url, strlen(MOREYIOT)) == 0) {
			morey_handler(url, wsi, found, r);
		}
		else if (strncmp(TODOURL, url, strlen(TODOURL)) == 0) {
			todo_handler(url, wsi, found, r);
		}
	}
	return n;
}

void getproj_handler(struct lws * wsi, bool * found, struct request * r) {
	// get url arguments
	char * url_arg = get_header_data(wsi, WSI_TOKEN_HTTP_URI_ARGS);

	char * pkg_name  = get_url_arg(url_arg, "pkg-name=");
	char * grid_name = get_url_arg(url_arg, "grid-name=");
	if (url_arg != NULL) {free(url_arg); url_arg = NULL;}
	jobject * obj = NULL;
	if (pkg_name != NULL && grid_name != NULL) {
		int count = strlen(pkg_name) + strlen(grid_name) + 1;
		char * href = malloc(count * sizeof(*href));
		sprintf(href, "%s%s", pkg_name, grid_name);
		if (href != NULL) {
			format_href(href);
			obj = get_project_view(href);
		}
		if (href != NULL) {free(href);}
	}
	if (pkg_name != NULL)  {free(pkg_name);}
	if (grid_name != NULL) {
		append_jobject(&obj, "gridName", TEXT, (data_t) {.txt = grid_name});
	}

	json_tostring(&r->buff, obj, &r->alloc_size);
	free_json(&obj);
	if ((*found = (r->buff != NULL))) {
		CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
	}
}

void createproj_handler(struct lws * wsi, bool * found, struct request * r) {
	jobject * robj = admin_auth(wsi);
	jobject * temp = get_jobject("canEdit", robj);
	bool is_valid = temp != NULL && temp->type == CON && temp->data.cond;

	char * url_arg   = get_header_data(wsi, WSI_TOKEN_HTTP_URI_ARGS);
	char * pkg_name  = get_url_arg(url_arg, "pkg-name=");
	char * grid_name = get_url_arg(url_arg, "grid-name="); 
	char * date_str  = NULL;
	if (url_arg != NULL) {free(url_arg);}
	if (is_valid) {
		date_str  = get_header_data(wsi, WSI_TOKEN_HTTP_EXPIRES);
	}

	is_valid = (pkg_name != NULL) && (grid_name != NULL) && (date_str != NULL);

	char * database_name = NULL;
	if (is_valid) {
		int num_char  = strlen(pkg_name) + strlen(grid_name) + strlen(date_str) + 1;
		database_name = malloc(sizeof(*database_name) * num_char);
	}

	if ((is_valid = (is_valid && database_name != NULL))) {
		sprintf(database_name, "%s%s%s", pkg_name, grid_name, date_str);
	}
	FREE(date_str);
	
	char tokenBuffer[11];
	if (database_name != NULL) {
		char tokenTemp[65];
		sha256_string(database_name, tokenTemp);
		tokenTemp[10] = '\0';		
		strcpy(tokenBuffer, tokenTemp);
		database_name[strlen(pkg_name) + strlen(grid_name)] = '\0';
		format_href(database_name);

		// add to database
		web_record_href(tokenTemp, false);
		insert_grid(database_name, NEWPAGE);
		set_project_view(database_name, tokenTemp, 100, "<h3>Title</h3><p>body</p>");

		FREE(database_name);

		// return to response
		char * tokenalloc = malloc(sizeof(*tokenalloc) * (strlen(tokenTemp) + 1));
		sprintf(tokenalloc, "%s", tokenTemp);
		append_jobject(&robj, "href", TEXT, (data_t) {.txt = tokenalloc});
	}
	else {
		tokenBuffer[0] = '\0';
	}
	FREE(pkg_name);
	FREE(grid_name);

	char * directory  = NULL; 
	if ((is_valid = (is_valid && tokenBuffer[0] != '\0'))) {
		int num_char = strlen(TEMPLATE_PARENT) + ID_LENGTH + MAX_FILE_LENGTH + 1;
		directory    = malloc(sizeof(*directory) * num_char);
	}

	if (directory != NULL) {
		// create www/other directory
		sprintf(directory, "%s", TEMPLATE_PARENT);
		mkdir(directory, S_IRWXU);
		sprintf(directory, "%s%s", TEMPLATE_PARENT, tokenBuffer);
		mkdir(directory, S_IRWXU);

		// create js file
		sprintf(directory, "%s%s%s", TEMPLATE_PARENT, tokenBuffer, TEMPLATE_MAINJS);
		FILE * fp, * index;
		if ((fp = fopen(directory, "w")) != NULL) {
			fprintf(fp, "const ref = \"%s\";\n", tokenBuffer);
			fclose(fp);
		}

		// create css file
		sprintf(directory, "%s%s%s", TEMPLATE_PARENT, tokenBuffer, TEMPLATE_CSS);
		if ((fp = fopen(directory, "w")) != NULL) {
			fprintf(fp, ".%s {\n }\n", tokenBuffer);
			fclose(fp);
		}
		
		// create html file
		sprintf(directory, "%s%s%s", TEMPLATE_PARENT, tokenBuffer, TEMPLATE_INDEX);
		if ((fp = fopen(directory, "w")) != NULL) {
			if ((index = fopen("www/html/template/template.html", "r")) != NULL) {
				// create index file
				char buffer[21];
				bool is_finished = false;
				for (size_t i = 0; !is_finished;) {
					memset(buffer, 0 , sizeof(buffer));
					if (!(is_finished = ((i = fread(buffer, sizeof(char), 20, index)) <= 0))) {
						fwrite(buffer, sizeof(char), i, fp);
					}
				}
				fprintf(fp, "<script src=\"www/other/%s/main.js\"></script>\n", tokenBuffer);
				fprintf(fp, "<link rel=\"stylesheet\" href=\"www/other/%s/main.css\"/>\n", tokenBuffer);
				// close file
				fclose(index);
			}
			fclose(fp);
		}

		FREE(directory);
	}

	json_tostring(&r->buff, robj, &r->alloc_size);
	free_json(&robj);
	if ((*found = (r->buff != NULL))) {
		CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
	}
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
