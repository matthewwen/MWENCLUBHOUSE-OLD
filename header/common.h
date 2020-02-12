#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libwebsockets.h>

typedef uint8_t free_t;
enum {
	BUFFER_STATIC,
	BUFFER_MALLOC,
};

typedef uint8_t request_t;
enum {
	FILE_REQUEST,
	BUFFER_REQUEST,
};

typedef uint8_t type_t;
enum {
	PUT,
	POST,
	GET
};

struct request{
	request_t response;
	size_t    alloc_size;
	char *    buff; 
	char *    pos; 
	free_t    free_type;
	type_t    type;
	struct lws_spa * spa;
	char * url;
};

#define FREE(VAR) do {\
	if (VAR != NULL) {\
		free(VAR);\
		VAR = NULL;\
	}\
}while(false);

#define CREATE_REQUEST(REQUEST, RESPONSE, ALLOC_SIZE, BUFF, FREE_TYPE) do {\
    REQUEST->response   = RESPONSE;\
    REQUEST->alloc_size = ALLOC_SIZE;\
    REQUEST->buff       = BUFF;\
    REQUEST->pos        = BUFF;\
    REQUEST->free_type  = FREE_TYPE;\
}while(false);

char * get_header_data(struct lws * wsi, enum lws_token_indexes h);
char * get_custom_header_data(struct lws * wsi, const char * token);
char * get_url_arg(const char * url_arg, const char * key);

#endif
