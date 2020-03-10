#ifndef _TODO_H
#define _TODO_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <libwebsockets.h>
#include "common.h"

// api url request
#define TODOURL "/todo"

void todo_handler(const char *, struct lws * wsi, bool * found, struct request * r);

#endif
