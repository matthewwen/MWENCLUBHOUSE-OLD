#include <libwebsockets.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

struct pss { struct lws_spa *spa; char filename[128]; unsigned long long file_length; int fd;};

static int interrupted;
static const char * const param_names[] = {
    "text1",
    "send",
};

enum enum_param_names {
    EPN_TEXT1,
    EPN_SEND,
};

static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    uint8_t buf[LWS_PRE + LWS_RECOMMENDED_MIN_HEADER_SPACE], *start = &buf[LWS_PRE], *p = start, *end = &buf[sizeof(buf) - 1];
    struct pss *pss = (struct pss *)user;
    int n;

    if (reason == LWS_CALLBACK_HTTP) {
        // getting the url
        printf("url: %s\n", (const char *) in);
        if (!strcmp((const char *)in, "/form1")) {
            return 0;
        }
    }
    else if (reason == LWS_CALLBACK_HTTP_BODY) {
        if (!pss->spa) {
            pss->spa = lws_spa_create(wsi, param_names, LWS_ARRAY_SIZE(param_names), 1024, NULL, pss);
            if (!pss->spa) {
                return -1;
            }
        }

        if (lws_spa_process(pss->spa, in, (int)len)) {
            return -1;
        }
    }
    else if (reason == LWS_CALLBACK_HTTP_BODY_COMPLETION) {
        lws_spa_finalize(pss->spa);

        for (n = 0; n < (int)LWS_ARRAY_SIZE(param_names); n++) {
            if (!lws_spa_get_string(pss->spa, n)) {
                lwsl_user("%s: undefined\n", param_names[n]);
            }
            else {
                lwsl_user("%s: (len %d) '%s'\n", param_names[n], lws_spa_get_length(pss->spa, n), lws_spa_get_string(pss->spa, n));
            }
        }

        printf("should redirect\n");
        if (lws_http_redirect(wsi, HTTP_STATUS_MOVED_PERMANENTLY, "/", 1, &p, end) < 0) {
            return -1;
        }
    }
    else if (reason == LWS_CALLBACK_HTTP_DROP_PROTOCOL) {
        printf("end pss->spa\n");
        if (pss->spa) {
            lws_spa_destroy(pss->spa);
            pss->spa = NULL;
        }
    }

    return lws_callback_http_dummy(wsi, reason, user, in, len);
}

static struct lws_protocols protocols[] = {
    { "http", callback_http, sizeof(struct pss), 0 },
    { NULL, NULL, 0, 0 } /* terminator */
};

/* default mount serves the URL space from ./mount-origin */
static const struct lws_http_mount mount = {NULL,"/", "./mount-origin","index.html",NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0,LWSMPRO_FILE,1,NULL,};

void sigint_handler(int sig) { interrupted = 1;}

int main(int argc, const char **argv) {
    struct lws_context_creation_info info;
    struct lws_context *context;
    const char *p;
    int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    signal(SIGINT, sigint_handler);

    if ((p = lws_cmdline_option(argc, argv, "-d"))) {
        logs = atoi(p);
    }

    lws_set_log_level(logs, NULL);
    lwsl_user("LWS minimal http server POST file | visit http://localhost:7681\n");

    memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
    info.port = 7681;
    info.protocols = protocols;
    info.mounts = &mount;
    info.options = 0;

    context = lws_create_context(&info);
    if (!context) {
        lwsl_err("lws init failed\n");
        return 1;
    }

    while (n >= 0 && !interrupted) {
        n = lws_service(context, 0);
    }
    lws_context_destroy(context);

    return 0;
}
