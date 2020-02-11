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

static const char * TEMPLATE_PARENT = "www/other/";
static const char * TEMPLATE_INDEX  = "/index.html";
static const char * TEMPLATE_MAINJS = "/main.js";
static const char * TEMPLATE_CSS    = "/main.css";

#define MAX_FILE_LENGTH  11
#define ID_LENGTH 10

void webedit_handler(struct lws * wsi, bool * found, struct request * r);
void createPROJ_handler(struct lws * wsi, bool * found, struct request * r);

int handle_gapi_request(const char * url, struct lws * wsi, bool * found, struct request * r) {
	int n = 0;
	if (*found == false) {
		if (strcmp("/webedit", url) == 0) {
			webedit_handler(wsi, found, r);
		}
		else if (strcmp("/mwenCreatePROJ", url) == 0) {
			createPROJ_handler(wsi, found, r);
		}
	}
	return n;
}

void createPROJ_handler(struct lws * wsi, bool * found, struct request * r) {
	jobject * robj = admin_auth(wsi);
	jobject * temp = get_jobject("canEdit", robj);
	bool is_valid = temp != NULL && temp->type == CON && temp->data.cond;

	char * pkg_name = NULL, * grid_name = NULL, * date_str = NULL;
	if (is_valid) {
		pkg_name  = get_custom_header_data(wsi, "pkg-name:");
		grid_name = get_custom_header_data(wsi, "grid-name:");
		date_str  = get_custom_header_data(wsi, "mwen-date:");
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
	
	char tokenBuffer[65];
	if (database_name != NULL) {
		sha256_string(database_name, tokenBuffer);
		tokenBuffer[10] = '\0';		
		database_name[strlen(pkg_name) + strlen(grid_name)] = '\0';
		for (int i = 0; database_name[i] != '\0'; i++) {
			if (database_name[i] == '-') {
				database_name[i] = '_';
			}
		}
		// create database
		sqlite3 * db = NULL;
		add_document(&db, "data.db", database_name, tokenBuffer);
		sqlite3_close(db);

		FREE(database_name);

		// return to response
		char * tokenalloc = malloc(sizeof(*tokenalloc) * (strlen(tokenBuffer) + 1));
		sprintf(tokenalloc, "%s", tokenBuffer);
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
