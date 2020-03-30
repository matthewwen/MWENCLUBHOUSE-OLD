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

// reference the href
void web_record_href(char * ref, bool is_visible);
jobject * get_web_record_href();
bool get_is_visible(char * href);
void set_is_visible(char * href, bool is_visible);

// finding grid table
typedef enum {NONE, NEWPAGE} grid_type_t;
void insert_grid(char * table_name, grid_type_t type);
void set_grid_type(char * table_name, grid_type_t type);
grid_type_t get_table_type(char * table_name);

// for project view
void set_project_view(char * table_name, char * ref, int weight, char * body);
jobject * get_project_view(char * table_name);
#endif
