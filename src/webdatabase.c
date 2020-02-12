#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "webdatabase.h"
#include "json.h"

#define PROPERTIES   "(Id INTEGER PRIMARY KEY, ref TEXT, visible INTEGER);"
#define WEBDATABASE  "webdata.db"
#define PAGEVIEW     "PAGEVIEW"

void web_record_href(char * ref, bool is_visible) {
    if (ref == NULL) {
        return;
    }

    sqlite3 * db = NULL;
    sqlite_err_t res = create_table(&db, WEBDATABASE, PAGEVIEW, PROPERTIES);
    SQLITE_ERROR_START(res);
    return;
    SQLITE_ERROR_END();

    char * insertstr = "INSERT INTO PAGEVIEW (ref, visible) VALUES('%s', %d)";
    int count = strlen(insertstr) + strlen(ref) + 2;
    char * buffer = malloc(count * sizeof(*buffer));
    if (buffer != NULL) {
        sprintf(buffer, insertstr, ref, (is_visible ? 1: 0));
        res = exec_task(&db, buffer);
        printf("buffer: %s\n", buffer);
        free(buffer);
    }

    if (db != NULL) {sqlite3_close(db); db = NULL;}
    
}

typedef struct {
    char * url;
    bool value;
    int id;
}href_find_t;

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
    create_table(&db, WEBDATABASE, PAGEVIEW, PROPERTIES);
    sqlite3_exec(db, "SELECT * FROM PAGEVIEW", href_handler, &list, NULL);
    jobject * obj = create_jobject("data", LIST, (data_t) {.list = list});
    if (db != NULL) {sqlite3_close(db); db = NULL;}
    return obj;
}

href_find_t get_href(char * href) {
    href_find_t h = (href_find_t) {.url = href, .value = false, .id = -1};
    if (href != NULL) {
        sqlite3 * db = NULL;
        create_table(&db, WEBDATABASE, PAGEVIEW, PROPERTIES);
        char * response_buffer = "SELECT * FROM PAGEVIEW WHERE ref='%s';";
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
    char * response_buffer = "UPDATE PAGEVIEW SET visible=%d WHERE Id=%d;";
    if (href != NULL && h.id > -1) {
        int count = (strlen(response_buffer) - 3) + 1;
        int id = h.id;
        for (; id > 0; id /= 10){count++;}
        char * response = malloc(count * sizeof(*response));

        if (response != NULL) {
            sprintf(response, response_buffer, (is_visible ? 1: 0), h.id);

            sqlite3 * db = NULL;
            sqlite_err_t res = create_table(&db, WEBDATABASE, PAGEVIEW, PROPERTIES);
            if (res == SQLITE_OK) {
                sqlite3_exec(db, response, NULL, NULL, NULL);
            }
            free(response);
            if (db != NULL) {sqlite3_close(db); db = NULL;}
        }
    }
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */