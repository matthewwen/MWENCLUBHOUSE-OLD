#include "Python.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

#include <czmq.h>
#include <zcert.h>

#include "gethandler.h"
#include "posthandler.h"
#include "puthandler.h"
#include "mqtthandler.h"
static int INTERRUPTED;
const char * PARAM[] = {"data", "auth"};

#define MAXBUFFERLEN (LWS_PRE + 2048)
#define MIN15 (60 * 15)

typedef struct{
	int n;
}handle_t;

void setup_python() {
#ifdef ALLOWPYTHON
	Py_Initialize();
	// import
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient");
	PyRun_SimpleString("import time");
	PyRun_SimpleString("import json");

	// read local scripts
	PyRun_SimpleString("sys.path.append(\"py\")");
	PyRun_SimpleString("import pub");	

	// start aws
	PyRun_SimpleString("a = pub.setup()");
	PyRun_SimpleString("running = 0");
#endif
}

void destroy_python() {
#ifdef ALLOWPYTHON
	// kill aws
	PyRun_SimpleString("pub.disconnect(a)");

	// kill python
	Py_Finalize();
#endif
}

static int callback_dynamic_http(struct lws *wsi, enum lws_callback_reasons reason,
		void *user, void *in, size_t len) {
	struct request *pss = (struct request *)user;
	uint8_t buf[LWS_PRE + 2048], *start = &buf[LWS_PRE], *p = start,
		*end = &buf[sizeof(buf) - LWS_PRE - 1];

	char msg_buffer[MAXBUFFERLEN];

	int n;
	if (reason == LWS_CALLBACK_HTTP) {

		lws_snprintf(pss->path, sizeof(pss->path), "%s", (const char *)in);


		if (lws_hdr_total_length(wsi, WSI_TOKEN_GET_URI)) {
			pss->type = GET;
			return handle_get_request(in, wsi, &user);
		}
		else if (lws_hdr_total_length(wsi, WSI_TOKEN_POST_URI)) {
			pss->type = POST;
		}
		else if (lws_hdr_total_length(wsi, WSI_TOKEN_PUT_URI)) {
			pss->type = PUT;
		}

		if (pss->type != GET) {
			char * url = in;
			char * alloc_url = malloc((strlen(url) + 1) * sizeof(*alloc_url));
			strcpy(alloc_url, url);
			pss->url = alloc_url;
		}

		return 0;

	}
	else if (reason == LWS_CALLBACK_HTTP_WRITEABLE) {
		n = LWS_WRITE_HTTP;
		if (!pss || pss->times > pss->budget) {
		}
		else {
			memset(msg_buffer, 0, sizeof(msg_buffer));
			size_t num_sent = pss->alloc_size;
			if (num_sent > MAXBUFFERLEN) {
				num_sent = MAXBUFFERLEN;
			}
			pss->alloc_size -= num_sent;

			if (pss->alloc_size <= 0) {
				n = LWS_WRITE_HTTP_FINAL;
			}
			memcpy(msg_buffer, pss->pos, num_sent);

			if (lws_write(wsi, (uint8_t *) msg_buffer, num_sent, n) != num_sent) {
				return 1;
			}

			pss->pos = pss->pos + (num_sent / sizeof(*pss->pos));

			if (n == LWS_WRITE_HTTP_FINAL && pss->free_type == BUFFER_MALLOC) {
				free(pss->buff);
			}

			if (n == LWS_WRITE_HTTP_FINAL && lws_http_transaction_completed(wsi)) {
				return -1;
			} else {
				lws_callback_on_writable(wsi);
			}
			
			return 0;
		}
	}
	else if (reason == LWS_CALLBACK_HTTP_BODY) {
		if (pss->type == PUT || pss->type == POST) {
			if (pss->spa == NULL) {
				pss->spa = lws_spa_create(wsi, PARAM, LWS_ARRAY_SIZE(PARAM), 1024, NULL, user);
			}
			if (lws_spa_process(pss->spa, in, (int) len)) {
				return -1;
			}
		}
	}
	else if (reason == LWS_CALLBACK_HTTP_BODY_COMPLETION) {
		if (pss->spa == NULL) {
		}
		else {
			lws_spa_finalize(pss->spa);
			const char * response = lws_spa_get_string(pss->spa, 0);
			if (pss->type == PUT || pss->type == POST) {
				if (pss->type == PUT) {
					handle_put_request(pss->url, wsi, &user);
				}
				else {
					handle_post_request(pss->url, wsi, &user);
				}
				if (pss->url != NULL) {
					free(pss->url);
					pss->url = NULL;
				}

				const char * getrequest = "/request";
				if (lws_http_redirect(wsi, HTTP_STATUS_OK, (const unsigned char *) getrequest, strlen(getrequest), &p, end) < 0) {
					return -1;
				}
			}
		}
	}
	else if (reason == LWS_CALLBACK_HTTP_DROP_PROTOCOL) {
		if (pss->spa != NULL) {
			lws_spa_destroy(pss->spa);
			pss->spa = NULL;
		}
	}

	return lws_callback_http_dummy(wsi, reason, user, in, len);
}


void sigint_handler(int sig) {printf("signal stopped\n"); INTERRUPTED = 1;}

int main(int argc, char* argv[]) {
	// set up python
	// TODO -> fix custom header!! printf("token to string: %s\n", lws_token_to_string(WSI_TOKEN_HTTP_EXPIRES));
	setup_python();	

	INTERRUPTED = 0;

	// LIBWEBSOCKETS SETUP
	const struct lws_protocols protocol = {"http", callback_dynamic_http, sizeof(struct request), 0};
	const struct lws_protocols *pprotocols[] = {&protocol, NULL };

	//override the default mount for /dyn in the URL space 
	const struct lws_http_mount mount_dyn = {NULL      , "/", NULL   , NULL        , "http", NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, LWSMPRO_CALLBACK, 4, NULL};
	const struct lws_http_mount mount     = {&mount_dyn, "/", "./www", "index.html", NULL  , NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, LWSMPRO_FILE    , 1, NULL};
#ifndef TESTDEPLOYMENT
	const struct lws_http_mount mount80   = {NULL, "/", "www.matthewwen.com", "/", NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, LWSMPRO_REDIR_HTTPS, 1, NULL,};
#endif

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

#ifndef TESTDEPLOYMENT
	info.options &= ~LWS_SERVER_OPTION_REDIRECT_HTTP_TO_HTTPS;
	info.port       = 80;
	info.mounts     = &mount80;
	info.vhost_name = "www.matthewwen.com";

	if (!lws_create_vhost(context, &info)) {
		lwsl_err("Failed to create tls vhost\n");
		lws_context_destroy(context);
		return EXIT_FAILURE;
	}

	info.port = 443;
	info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT | LWS_SERVER_OPTION_EXPLICIT_VHOSTS;
	info.pprotocols = pprotocols;
	info.mounts = &mount;
	info.error_document_404 = "/html/404.html";
	info.ssl_cert_filepath = "/etc/letsencrypt/live/www.matthewwen.com/fullchain.pem";
	info.ssl_private_key_filepath = "/etc/letsencrypt/live/www.matthewwen.com/privkey.pem";
	info.vhost_name = "www.matthewwen.com";

	if (!lws_create_vhost(context, &info)) {
		lwsl_err("Failed to create tls vhost\n");
		lws_context_destroy(context);
		return EXIT_FAILURE;
	}
#else
	info.options = LWS_SERVER_OPTION_EXPLICIT_VHOSTS;
	info.port       = 8888;
	info.pprotocols = pprotocols;
	info.mounts     = &mount;
	info.vhost_name = "192.168.0.21";

	if (!lws_create_vhost(context, &info)) {
		lwsl_err("Failed to create tls vhost\n");
		lws_context_destroy(context);
		return EXIT_FAILURE;
	}
#endif

	// MQTT SETUP
	int mqtt_n = -1;
	char mqtt_buff[MQTT_MAX_LENGTH];
	void * mqtt_context = zmq_ctx_new();
	void * mqtt_responder = zmq_socket(context, ZMQ_REP);
	mqtt_n = zmq_bind(mqtt_responder, "tcp://*:1883");

#ifdef ALLOWPYTHON
	//PyRun_SimpleString("a = pub.subscribe(a)");
#endif
	for (int i = 0; n >= 0 && !INTERRUPTED; i++) {
		// libwebsockets
		n = lws_service(context, 0);

		// mqtt broker
		memset(mqtt_buff, 0, sizeof(mqtt_buff));
		mqtt_n = zmq_recv(mqtt_responder, mqtt_buff, MQTT_MAX_LENGTH - 1, ZMQ_DONTWAIT);
		handle_mqtt_request(mqtt_n, mqtt_context, mqtt_responder, mqtt_buff);

#ifdef ALLOWPYTHON
		if ((i = (i % MIN15)) == 0) {
			//PyRun_SimpleString("a = pub.publish(a, \"update\")");
		}
#endif
	}

	// destroy web server
	lws_context_destroy(context);

	// destroy python
	destroy_python();

	// destroy mqtt broker
	zmq_close(mqtt_responder);
	zmq_ctx_destroy(mqtt_context);

	return EXIT_SUCCESS;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
