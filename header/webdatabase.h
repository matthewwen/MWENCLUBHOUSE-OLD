#ifndef _WEB_DATABASE_
#define _WEB_DATABASE_
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdbool.h>
#include "msqlite.h"
#include "json.h"

#define PAGETABLE "pageView"
#define IMGTABLE  "imageView"

void web_record_href(char * ref, bool is_visible);
void get_web_record_href(sqlite3 ** db);

#endif
