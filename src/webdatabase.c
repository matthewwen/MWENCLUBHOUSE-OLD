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

void get_web_record_href(sqlite3 ** db) {
    sqlite_err_t res = create_table(db, WEBDATABASE, PAGEVIEW, PROPERTIES);

    SQLITE_ERROR_START(res);
    return;
    SQLITE_ERROR_END();
}

/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
