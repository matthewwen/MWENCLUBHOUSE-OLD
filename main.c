#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "gethandler.h"
#include "posthandler.h"
#include "puthandler.h"
static int INTERRUPTED;
const char * PARAM[] = {"data", "auth"};

#define MAXBUFFERLEN 1000000

typedef struct{
	int n;
}handle_t;

int callback_dynamic_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	uint8_t buf[LWS_PRE + LWS_RECOMMENDED_MIN_HEADER_SPACE];
	memset(buf, 0, sizeof(buf));
	uint8_t * start = &buf[LWS_PRE];
	uint8_t * p     = start;
	uint8_t * end   = &buf[sizeof(buf) - 1];

	struct request * r = user;
	if (reason == LWS_CALLBACK_HTTP) {
		if (lws_hdr_total_length(wsi, WSI_TOKEN_GET_URI)) {
			r->type = GET;
			return handle_get_request(in, wsi, &user);
		}
		else if (lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)) {
			r->type = POST;
		}
		else if (lws_hdr_total_length(wsi, WSI_TOKEN_PUT_URI)) {
			r->type = PUT;
		}

		if (r->type != GET) {
			char * url = in;
			char * alloc_url = malloc((strlen(url) + 1) * sizeof(*alloc_url));
			strcpy(alloc_url, url);
			r->url = alloc_url;
		}
		return 0;
	}
	else if (reason == LWS_CALLBACK_HTTP_WRITEABLE) {
		size_t num_sent = r->alloc_size;
		if (num_sent > MAXBUFFERLEN) {
			num_sent = MAXBUFFERLEN;
		}
		lws_write(wsi, (uint8_t *) r->pos, num_sent, LWS_WRITE_HTTP_FINAL);

		r->alloc_size -= num_sent;
		if (r->alloc_size <= 0) {
			if (r->free_type == BUFFER_MALLOC) {
				free(r->buff);
			}

			if (lws_http_transaction_completed(wsi)) {
				return -1;
			}
			return -1;
		}
		else {
			r->pos += num_sent;
			lws_callback_on_writable(wsi);
			return 0;
		}
	}
	else if (reason == LWS_CALLBACK_HTTP_BODY) {
		if (r->type == PUT || r->type == POST) {
			if (r->spa == NULL) {
				r->spa = lws_spa_create(wsi, PARAM, LWS_ARRAY_SIZE(PARAM), 1024, NULL, user);
			}
			if (lws_spa_process(r->spa, in, (int) len)) {
				return -1;
			}
		}
	}
	else if (reason == LWS_CALLBACK_HTTP_BODY_COMPLETION) {
		if (r->spa == NULL) {
		}
		else {
			lws_spa_finalize(r->spa);
			const char * response = lws_spa_get_string(r->spa, 0);
			printf("response: %s\n", response);
			if (r->type == PUT || r->type == POST) {
				if (r->type == PUT) {
					handle_put_request(r->url, wsi, &user);
				}
				else {
					handle_post_request(r->url, wsi, &user);
				}
				if (r->url != NULL) {
					free(r->url);
					r->url = NULL;
				}

				const char * getrequest = "/request";
				if (lws_http_redirect(wsi, HTTP_STATUS_OK, (const unsigned char *) getrequest, strlen(getrequest), &p, end) < 0) {
					return -1;
				}
			}
		}
	}
	else if (reason == LWS_CALLBACK_HTTP_DROP_PROTOCOL) {
		if (r->spa != NULL) {
			lws_spa_destroy(r->spa);
			r->spa = NULL;
		}
	}

	return lws_callback_http_dummy(wsi, reason, user, in, len);
}

void sigint_handler(int sig) {printf("signal stopped\n"); INTERRUPTED = 1;}

int main(int argc, char* argv[]) {
	INTERRUPTED = 0;
	const struct lws_protocols protocol = {"http", callback_dynamic_http, sizeof(struct request), 0};
	const struct lws_protocols *pprotocols[] = {&protocol, NULL };

	//override the default mount for /dyn in the URL space 
	const struct lws_http_mount mount_dyn = {NULL      , "/", NULL   , NULL        , "http", NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, LWSMPRO_CALLBACK, 4, NULL};
	const struct lws_http_mount mount     = {&mount_dyn, "/", "./www", "index.html", NULL  , NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, LWSMPRO_FILE    , 1, NULL};

	int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
	signal(SIGINT, sigint_handler);
	lws_set_log_level(logs, NULL);

	struct lws_context_creation_info info;
	memset(&info, 0, sizeof(info));
	info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT | LWS_SERVER_OPTION_EXPLICIT_VHOSTS;

	struct lws_context * context = lws_create_context(&info);
	if (!context) {
		lwsl_err("lws init failed\n");
		return EXIT_FAILURE;
	}
	info.port       = 80;
	info.pprotocols = pprotocols;
	info.mounts     = &mount;
	info.vhost_name = "http";

	if (!lws_create_vhost(context, &info)) {
		lwsl_err("Failed to create tls vhost\n");
		lws_context_destroy(context);
		return EXIT_FAILURE;
	}

	if (!lws_create_vhost(context, &info)) {
		lwsl_err("Failed to create tls vhost\n");
		lws_context_destroy(context);
		return EXIT_FAILURE;
	}

	while (n >= 0 && !INTERRUPTED) {
		n = lws_service(context, 0);
	}
	lws_context_destroy(context);

	return EXIT_SUCCESS;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
