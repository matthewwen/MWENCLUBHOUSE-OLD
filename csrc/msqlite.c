#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "msqlite.h"

#define COMMAND_LIMIT 2000

typedef struct {
    jobject * head;
    int type;
}jobject_holder_t;

sqlite_err_t create_table(sqlite3 ** db, char * database_name, char * table_name, const char * format) {
    sqlite_err_t res = SQLITE_OK;
    if (*db == NULL) {
        res = open_db(db, database_name);
    }
    SQLITE_ERROR_START(res);
    return res;
    SQLITE_ERROR_END();

    const char * request_buffer = "CREATE TABLE IF NOT EXISTS %s %s";
    int num_count = strlen(request_buffer) + strlen(table_name) + strlen(format) + 1;
    char * sql_create = malloc(num_count * sizeof(*sql_create));
    sprintf(sql_create, request_buffer, table_name, format);

    res = exec_task(db, sql_create);
    free(sql_create);
    SQLITE_ERROR_HANDLE(res);

    return res;
}

int read_data(void * notused, int argc, char ** argv , char ** azColName) { 
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    return 0;
}

sqlite_err_t parse_data(sqlite3 ** db, char * database_name, char * table_name, jobject ** a_head) { 
    sqlite_err_t res = SQLITE_OK;
    return res;
}

sqlite_err_t open_db(sqlite3 ** db, char * databse_name) {
    sqlite_err_t res = sqlite3_open(databse_name, db);
    SQLITE_ERROR_START(res);
    sqlite3_close(*db);
    *db = NULL;
    SQLITE_ERROR_END();
    return res;
}

sqlite_err_t exec_task(sqlite3 ** db, char * task) {
    sqlite_err_t res = *db == NULL ? SQLITE_FAIL: SQLITE_OK;
    SQLITE_ERROR_START(res);
    return res;
    SQLITE_ERROR_END();

    char * error_msg = NULL;
    res = sqlite3_exec(*db, task, 0, 0, &error_msg);
    SQLITE_ERROR_START(res);
    printf("Error id: %d, Error msg: %s\n", res, error_msg);
    sqlite3_free(error_msg);
    sqlite3_close(*db);
    *db = NULL;
    SQLITE_ERROR_END();

    return res;
}

sqlite_err_t drop_table(sqlite3 ** db, char * table_name) {
    return 0;
}
