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

static const char * TEMPLATE_PARENT = "www/other/";
static const char * TEMPLATE_INDEX  = "/index.html";
static const char * TEMPLATE_MAINJS = "/main.js";
static const char * TEMPLATE_CSS    = "/main.css";
#define MAX_FILE_LENGTH  11

#define FREE(VAR) do {\
	if (VAR != NULL) {\
		free(VAR);\
		VAR = NULL;\
	}\
}while(false);

void createPROJ_handler(struct lws * wsi, bool * found, struct request * r) {
	jobject * robj = admin_auth(wsi);
	jobject * temp = get_jobject("canEdit", robj);
	if (temp == NULL || temp->type != CON || temp->data.cond == false) {
		free_json(&robj);
		return;
	}

	/*char * pkg_name  = get_custom_header_data(wsi, "pkg-name:");
	char * grid_name = get_custom_header_data(wsi, "grid-name:");
	char * date_str  = get_custom_header_data(wsi, "mwen-date:");

	bool is_valid = (pkg_name != NULL) || (grid_name != NULL) || (date_str != NULL);

	char * database_name = NULL;
	if (is_valid == true) {
		int num_char  = strlen(pkg_name) + strlen(grid_name) + strlen(date_str) + 1;
		database_name = malloc(sizeof(*database_name) * num_char);
		is_valid = database_name != NULL;
	}

	if (is_valid == true) {
		sprintf(database_name, "%s%s%s", pkg_name, grid_name, date_str);
	}
	FREE(pkg_name);
	FREE(grid_name);
	FREE(date_str);
	
	if (is_valid == false) {
		free_json(&robj);
		FREE(database_name)
		return;
	}

	char tokenBuffer[65];
	memset(tokenBuffer, 0, sizeof(tokenBuffer));
	sha256_string(database_name, tokenBuffer);
	FREE(database_name);
	tokenBuffer[16] = '\0';

	size_t alloc_size = sizeof(char) * (strlen(TEMPLATE_PARENT) + 16 + MAX_FILE_LENGTH);
	char * directory  = malloc(alloc_size);
	if (directory == false) {
		free_json(&robj);
		return;
	}

	memset(directory, 0, alloc_size);
	sprintf(directory, "%s%s", TEMPLATE_PARENT, tokenBuffer);
	mkdir(directory, S_IRWXU);

	sprintf(directory, "%s%s%s", TEMPLATE_PARENT, tokenBuffer, TEMPLATE_MAINJS);
	FILE * fp = fopen(directory, "w");
	if (fp != NULL) {
		fclose(fp);
	}

	sprintf(directory, "%s%s%s", TEMPLATE_PARENT, tokenBuffer, TEMPLATE_CSS);
	fp = fopen(directory, "w");
	if (fp != NULL) {
		fclose(fp);
	}
	sprintf(directory, "%s%s%s", TEMPLATE_PARENT, tokenBuffer, TEMPLATE_INDEX);
	fp = fopen(directory, "w");
	if (fp != NULL) {
		FILE * fpOld = fopen("www/html/template/template.html", "r");
		if (fpOld != NULL) {
			char buffer[21];
			memset(buffer, 0 , sizeof(buffer));
			for (long i = 0;(i = fread(buffer, sizeof(char), 20, fpOld)) > 0;) {
				fwrite(buffer, sizeof(char), i, fp);
				memset(buffer, 0 , sizeof(buffer));
			}
			fclose(fpOld);
		}
		fclose(fp);
	}*/

	json_tostring(&r->buff, robj, &r->alloc_size);
	free_json(&robj);
	CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
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
