#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"
#include "json.h"
#include "apihandler.h"
#include "security.h"

char * get_header_data(struct lws * wsi, enum lws_token_indexes h) {
	char * str = NULL;
	int count = lws_hdr_total_length(wsi, h) + 1;
	if (count > 1) {
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
	if (count > 1) {
		str = malloc(count * sizeof(*str));
		memset(str, 0, count * sizeof(*str));
		lws_hdr_custom_copy(wsi, str, count, token, token_len);
	}
	return str;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */