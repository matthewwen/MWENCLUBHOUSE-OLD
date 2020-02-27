#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"
#include "json.h"
#include "apihandler.h"
#include "security.h"

#define BYTE_LIMIT 50000

char * get_header_data(struct lws * wsi, enum lws_token_indexes h) {
	char * str = NULL;
	int count = lws_hdr_total_length(wsi, h) + 1;
	if (count > 1 && count < BYTE_LIMIT) {
		str = malloc(count * sizeof(*str));
		if (str != NULL) {
			memset(str, 0, count * sizeof(*str));
			lws_hdr_copy(wsi, str, count, h);
		}
	}
	return str;
}

char * get_custom_header_data(struct lws * wsi, const char * token) {
	char * str = NULL;
	int token_len = token != NULL ? strlen(token): 0;
	int count = lws_hdr_custom_length(wsi, token, token_len) + 1;
	if (count > 1 && count < BYTE_LIMIT) {
		str = malloc(count * sizeof(*str));
		if (str != NULL) {
			memset(str, 0, count * sizeof(*str));
			lws_hdr_custom_copy(wsi, str, count, token, token_len);
		}
	}
	return str;
}

char * get_url_arg(const char * url_arg, const char * key) {
	char * str = NULL;
	if (url_arg != NULL && key != NULL) {
		char * keyloc  = strstr(url_arg, key);
		if (keyloc != NULL) {
			keyloc    += strlen(key);
			char * end = strchr(keyloc, ',');
			int count  = (end != NULL ? (end - keyloc) / sizeof(*str): strlen(keyloc)) + 1;
			size_t alloc_size = count * sizeof(*str);
			str = malloc(alloc_size);
			if (str != NULL) {
				memset(str, 0, alloc_size);
				strncpy(str, keyloc, alloc_size - sizeof(*str));
			}
		}
	}
	return str;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
