#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "apihandler.h"
#include "json.h"
#include "security.h"

char * get_header_data(struct lws * wsi, enum lws_token_indexes h) {
	char * str = NULL;
	int count = lws_hdr_total_length(wsi, h) + 1;
	if (count != 0) {
		str = malloc(count * sizeof(*str));
		memset(str, 0, count * sizeof(*str));
		lws_hdr_copy(wsi, str, count, h);
	}
	return str;
}

char * get_custom_header_data(struct lws * wsi, const char * token) {
	char * str = NULL;
	int token_len = strlen(token);
	int count = lws_hdr_custom_length(wsi, token, token_len) + 1;
	if (count != 0) {
		str = malloc(count * sizeof(*str));
		memset(str, 0, count * sizeof(*str));
		lws_hdr_custom_copy(wsi, str, count, token, token_len);
	}
	return str;
}

int handle_gapi_request(const char * url, struct lws * wsi, bool * found, struct request * r) {
	int n = 0;
	if (*found == false) {
		size_t alloc_size = 0;
		char * response = NULL;
		jobject * robj = NULL;
		if (strcmp("/webedit", url) == 0) {
			char * token = get_header_data(wsi, WSI_TOKEN_HTTP_AUTHORIZATION);
			free(token);

			char * date = get_custom_header_data(wsi, "mwen-date:");
			long time_date = convert_time(get_time()) - convert_time(get_time_from_buffer(date));
			time_date = time_date < 0 ? time_date * -1: time_date;


			if (time_date < 10000) {
				char * alloc_date = malloc((strlen(date) + 1) * sizeof(*date));
				strcpy(alloc_date, date);
				jobject * jobj = create_jobject("time", TEXT, (data_t) {.txt = alloc_date});

				// get password
				char * password = "kushal";
				char * alloc_password = malloc((strlen(password) + 1) * sizeof(*alloc_password));
				strcpy(alloc_password, password);
				append_jobject(&jobj, "password", TEXT, (data_t) {.txt = alloc_password});

				// get pull string
				char * json_str = NULL;
				json_tostring(&json_str, jobj, &alloc_size);

				char tokenBuffer[65];
				memset(tokenBuffer, 0, sizeof(tokenBuffer));
				sha256_string(json_str, tokenBuffer);

				// token
				robj = create_jobject("canEdit", CON, (data_t) {.cond = true});

				free_json(&jobj);
				free(json_str);
			}

			if (date != NULL) {
				free(date);
			}

			if (robj != NULL) {
				json_tostring(&response, robj, &alloc_size);
				alloc_size -= 1;
				*found = true;
				free_json(&robj);
			}

			CREATE_REQUEST(r, BUFFER_REQUEST, alloc_size, response, BUFFER_MALLOC);
		}
	}
	return n;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
