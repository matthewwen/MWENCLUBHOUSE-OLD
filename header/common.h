#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>

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
};

#endif
