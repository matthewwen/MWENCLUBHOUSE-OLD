#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "gethandler.h"
static int INTERRUPTED;

typedef struct{
    int n;
}handle_t;

int callback_dynamic_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    if (reason == LWS_CALLBACK_HTTP) {
        if (lws_hdr_total_length(wsi, WSI_TOKEN_GET_URI)) {
            return handle_get_request(in, wsi);
        }
        return 0;
    }

    return lws_callback_http_dummy(wsi, reason, user, in, len);
}

void sigint_handler(int sig) {
    printf("signal stopped\n");
    INTERRUPTED = 1;
}

int main(int argc, char* argv[]) {
    INTERRUPTED = 0;
    const struct lws_protocols protocol = {"http", callback_dynamic_http, NULL, 0};
    const struct lws_protocols *pprotocols[] = {&protocol, NULL };

    //override the default mount for /dyn in the URL space 
    const struct lws_http_mount mount_dyn = {NULL      , "/", NULL   , NULL        , "http", NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, LWSMPRO_CALLBACK, 4, NULL};
    const struct lws_http_mount mount     = {&mount_dyn, "/", "./www", "index.html", NULL  , NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, LWSMPRO_FILE    , 1, NULL};

    int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    signal(SIGINT, sigint_handler);
    lws_set_log_level(logs, NULL);
    lwsl_user("LWS minimal http server dynamic | visit http://localhost:80\n");

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof info);
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
        return EXIT_SUCCESS;
    }

    if (!lws_create_vhost(context, &info)) {
        lwsl_err("Failed to create tls vhost\n");
        lws_context_destroy(context);
        return EXIT_SUCCESS;
    }

    while (n >= 0 && !INTERRUPTED) {
        n = lws_service(context, 0);
    }
    lws_context_destroy(context);

    return EXIT_SUCCESS;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
