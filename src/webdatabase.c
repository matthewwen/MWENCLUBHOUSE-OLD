#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <assert.h>
#include "webdatabase.h"
#include "json.h"

#define PAGEVIEW            "PAGEVIEW"
#define PAGEVIEWPROPERTIES  "(Id INTEGER PRIMARY KEY, ref TEXT, visible INTEGER);"

#define TABLEVIEW           "TABLEVIEW"
#define TABLEVIEWPROPERTIES "(Id INTEGER PRIMARY KEY, name TEXT, type INTEGER);"
#define WEBDATABASE          "webdata.db"

#define NEWPAGEPROPERTIES "(Id INTEGER PRIMARY KEY, ref TEXT, weight INTEGER, body TEXT);"

int get_num_char(int val) {
	int count = val <= 0 ? 1: 0;
	val       = val < 0 ? -1 * val: val;
	for (; val > 0; val /= 10) {count++;}
	return count;
}

void web_record_href(char * ref, bool is_visible) {
	if (ref == NULL) {
		return;
	}

	sqlite3 * db = NULL;
	sqlite_err_t res = create_table(&db, WEBDATABASE, PAGEVIEW, PAGEVIEWPROPERTIES);
	SQLITE_ERROR_START(res);
	return;
	SQLITE_ERROR_END();

	char * insertstr = "INSERT INTO "PAGEVIEW" (ref, visible) VALUES('%s', %d)";
	int count = strlen(insertstr) + strlen(ref) + 2;
	char * buffer = malloc(count * sizeof(*buffer));
	if (buffer != NULL) {
		sprintf(buffer, insertstr, ref, (is_visible ? 1: 0));
		res = exec_task(&db, buffer);
		free(buffer);
	}

	if (db != NULL) {sqlite3_close(db); db = NULL;}
}

typedef struct {
	char * url;
	bool value;
	int id;
}href_find_t;

int all_handler(void * notused, int argc, char ** argv , char ** azColName) { 
	// declear variables
	jlist  ** a_list = notused;
	jlist   * list   = *a_list;
	jobject * obj    = NULL;

	for (int i = 0; i < argc; i++) {
		if (i == 0) {
			int id = atoi(argv[0]);
			append_jobject(&obj, azColName[0], NUM, (data_t) {.num = id});
		}
		else {
			char * str = malloc((strlen(argv[i]) + 1) * sizeof(*str));
			strcpy(str, argv[i]);
			append_jobject(&obj, azColName[i], TEXT, (data_t) {.txt = str});
		}
	}

	if (obj != NULL) {
		append_jlist(&list, OBJ, (data_t) {.obj = obj});
	}
	*a_list = list;
	return 0;
}

int href_handler(void * notused, int argc, char ** argv , char ** azColName) { 
	// declear variables
	jlist  ** a_list = notused;
	jlist   * list   = *a_list;
	jobject * obj    = NULL;

	// get visibility
	int var_database = atoi(argv[2]);
	bool is_visible = var_database != 0;
	append_jobject(&obj, "visible", CON, (data_t) {.cond = is_visible});

	// get href
	int count = strlen(argv[1]) + 1;
	char * str = malloc(count * sizeof(*str));
	strcpy(str, argv[1]);
	append_jobject(&obj, "href", TEXT, (data_t) {.txt = str});

	// get id
	int id = atoi(argv[0]);
	append_jobject(&obj, "id", NUM, (data_t) {.num = id});

	append_jlist(&list, OBJ, (data_t) {.obj = obj});
	*a_list = list;
	return 0;
}

int href_find_handler(void * notused, int argc, char ** argv, char ** azColName) {
	href_find_t * h = notused;
	if (strcmp(h->url, argv[1]) == 0) {
		int var_database = atoi(argv[2]);
		h->value = var_database != 0;
		h->id = atoi(argv[0]);
	}
	return 0;
}

jobject * get_web_record_href() {
	jlist * list = NULL;
	sqlite3 * db = NULL;
	create_table(&db, WEBDATABASE, PAGEVIEW, PAGEVIEWPROPERTIES);
	sqlite3_exec(db, "SELECT * FROM "PAGEVIEW, href_handler, &list, NULL);
	jobject * obj = create_jobject("data", LIST, (data_t) {.list = list});
	if (db != NULL) {sqlite3_close(db); db = NULL;}
	return obj;
}

href_find_t get_href(char * href) {
	href_find_t h = (href_find_t) {.url = href, .value = false, .id = -1};
	if (href != NULL) {
		sqlite3 * db = NULL;
		create_table(&db, WEBDATABASE, PAGEVIEW, PAGEVIEWPROPERTIES);
		char * response_buffer = "SELECT * FROM "PAGEVIEW" WHERE ref='%s';";
		int num_char = strlen(href) + strlen(response_buffer) + 1;
		char * response = malloc(num_char * sizeof(*response));
		if (response != NULL && db != NULL) {
			sprintf(response, response_buffer, href);
			sqlite3_exec(db, response, href_find_handler, &h, NULL);
		}
		if (db != NULL) {sqlite3_close(db); db = NULL;}
		if (response != NULL) {free(response);}
	}
	return h;
}

bool get_is_visible(char * href) {
	href_find_t h = get_href(href);
	return h.value;
}

void set_is_visible(char * href, bool is_visible) {
	href_find_t h = get_href(href);
	char * response_buffer = "UPDATE "PAGEVIEW" SET visible=%d WHERE Id=%d;";
	if (href != NULL && h.id > -1) {
		int count = (strlen(response_buffer) - 3) + 1;
		int id = h.id;
		for (; id > 0; id /= 10){count++;}
		char * response = malloc(count * sizeof(*response));

		if (response != NULL) {
			sprintf(response, response_buffer, (is_visible ? 1: 0), h.id);

			sqlite3 * db = NULL;
			sqlite_err_t res = create_table(&db, WEBDATABASE, PAGEVIEW, PAGEVIEWPROPERTIES);
			if (res == SQLITE_OK) {
				sqlite3_exec(db, response, NULL, NULL, NULL);
			}
			free(response);
			if (db != NULL) {sqlite3_close(db); db = NULL;}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
typedef struct{
	char * grid_name; 
	grid_type_t type;
	bool found;
	int id;
}grid_find_t;

int grid_find_handler(void * notused, int argc, char ** argv, char ** azColName) {
	grid_find_t * h = notused;
	if (strcmp(h->grid_name, argv[1]) == 0) {
		h->type  = atoi(argv[2]);
		h->found = true;
		h->id    = atoi(argv[0]);
	}
	return 0;
}

grid_find_t get_grid(char * table_name) {
	grid_find_t h = (grid_find_t) {.type = NONE, .grid_name = table_name, .found = false};
	h.id = 0;
	if (table_name != NULL) {
		sqlite3 * db = NULL;
		create_table(&db, WEBDATABASE, TABLEVIEW, TABLEVIEWPROPERTIES);
		char * response_buffer = "SELECT * FROM "TABLEVIEW" WHERE name='%s';";
		int num_char = strlen(table_name) + strlen(response_buffer) + 1;
		char * response = malloc(num_char * sizeof(*response));
		if (response != NULL && db != NULL) {
			sprintf(response, response_buffer, table_name);
			sqlite3_exec(db, response, grid_find_handler, &h, NULL);
		}
		if (db != NULL) {sqlite3_close(db); db = NULL;}
		if (response != NULL) {free(response);}
	}
	return h;
}

grid_type_t get_table_type(char * table_name) {
	grid_find_t h = get_grid(table_name);
	return h.type;
}

void insert_grid(char * table_name, grid_type_t type) {
	if (table_name == NULL || type == NONE) {
		return;
	}
	if (get_grid(table_name).found) {
		set_grid_type(table_name, type);
		return;
	}
	sqlite3 * db = NULL;
	sqlite_err_t res = create_table(&db, WEBDATABASE, TABLEVIEW, TABLEVIEWPROPERTIES);
	SQLITE_ERROR_START(res);
	return;
	SQLITE_ERROR_END();

	char * insertstr = "INSERT INTO "TABLEVIEW" (name, type) VALUES('%s', %d);";

	// get num char
	int count = strlen(insertstr) + strlen(table_name) + 2;
	for (int cptype = type; cptype > 0; cptype /= 10) {count++;}
	char * buffer = malloc(count * sizeof(*buffer));

	if (buffer != NULL) {
		sprintf(buffer, insertstr, table_name, type);
		res = exec_task(&db, buffer);
		free(buffer);
	}

	if (db != NULL) {sqlite3_close(db); db = NULL;}
}

void set_grid_type(char * table_name, grid_type_t type) {
	grid_find_t h = get_grid(table_name);
	char * response_buffer = "UPDATE TABLE SET type=%d WHERE Id=%d;";
	if (h.found) {
		int count = (strlen(response_buffer) - 3) + 2;
		for (int id = h.id; id > 0; id /= 10) {count++;}
		for (int id = h.type; id > 0; id /= 10) {count++;}
		char * response = malloc(count * sizeof(*response));

		if (response != NULL) {
			sprintf(response, response_buffer, type, h.id);

			sqlite3 * db = NULL;
			sqlite_err_t res = create_table(&db, WEBDATABASE, TABLEVIEW, TABLEVIEWPROPERTIES);
			if (res == SQLITE_OK) {
				sqlite3_exec(db, response, NULL, NULL, NULL);
			}
			free(response);
			if (db != NULL) {sqlite3_close(db); db = NULL;}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void set_project_view(char * table_name, char * ref, int weight, char * body) {
	char * insertstr = "INSERT INTO %s (ref, weight, body) VALUES('%s', %d, '%s');";
	int count = get_num_char(weight) + strlen(insertstr) + strlen(ref); 
	count     = count + strlen(body) + strlen(table_name) + 1;

	char * response = malloc(count * sizeof(*response));
	sprintf(response, insertstr, table_name, ref, weight, body);

	sqlite3 * db = NULL;
	sqlite_err_t res = create_table(&db, WEBDATABASE, table_name, NEWPAGEPROPERTIES);
	if (res == SQLITE_OK && response != NULL) {
		sqlite3_exec(db, response, NULL, NULL, NULL);
	}

	if (response != NULL) {free(response);}
	if (db != NULL) {sqlite3_close(db); db = NULL;}
}

jobject * get_project_view(char * table_name) {
	// check if grid exists
	grid_find_t h = get_grid(table_name);
	if (h.found == false) {
		return NULL;
	}

	jlist * list = NULL;
	// create database
	sqlite3 * db = NULL;
	create_table(&db, WEBDATABASE, table_name, NEWPAGEPROPERTIES);

	char * format = "SELECT * FROM %s;";
	int count = (strlen(format) - 2) + strlen(table_name) + 1;
	char * response = malloc(count * sizeof(*response));
	sprintf(response, format, table_name);

	if (db != NULL) {
		sqlite3_exec(db, response, all_handler, &list, NULL);
	}

	// free data
	if (db != NULL) {sqlite3_close(db); db = NULL;}
	if (response != NULL) {free(response); }

	return create_jobject("data", LIST, (data_t) {.list = list});
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
