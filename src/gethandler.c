#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "gethandler.h"
#include "webhandler.h"
#include "apihandler.h"
#include "common.h"
#include "security.h"

static char UNKNOWN[] = "{\"result\": \"unknown\"}";

int handle_get_request(const char * url, struct lws * wsi, void ** a_request) {
	int n = 0;
	bool request_found = false;
	struct request * r = *a_request;
	r->type = GET;

	// O(n) -> n = number of requests -> TODO -> change to hash map
	if (r->response == 0) {
		n = handle_gweb_request(url, wsi, &request_found, r);
		n = handle_gapi_request(url, wsi, &request_found, r);

		// request is not found, send unknown response 
		if (request_found == false) {
			CREATE_REQUEST(r, BUFFER_REQUEST, sizeof(UNKNOWN) - 1, UNKNOWN, BUFFER_STATIC);
		}

		char * mime = r->mime;
		if (r->response == BUFFER_REQUEST && r->buff != NULL) {
			mime = "plain/text";

			// TODO writing header part 1
			uint8_t * buff = r->header;
			uint8_t * end  = &r->header[HEADER_SIZE - 1];
			if (lws_add_http_common_headers(wsi, HTTP_STATUS_OK, mime, LWS_ILLEGAL_HTTP_CONTENT_LEN, &buff, end)) {
				n = 1;
			}

			// TODO check if this is correct part 2
			if (n != 1 && lws_finalize_write_http_header(wsi, r->header, &buff, end)) {
				n = 1;
			}

			if (r->response == BUFFER_REQUEST && r->buff != NULL && n != 1) {
				lws_callback_on_writable(wsi);
			}
		}

	}

	return n;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
