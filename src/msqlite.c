#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "msqlite.h"

#define COMMAND_LIMIT 2000

enum {NONE, DOCNUM, SECNUM, PROJNUM};

typedef struct {
    jobject * head;
    int type;
}jobject_holder_t;

sqlite_err_t open_db(sqlite3 ** db, char * databse_name) {
    sqlite_err_t res = sqlite3_open(databse_name, db);

    SQLITE_ERROR_START(res);
    sqlite3_close(*db);
    *db = NULL;
    SQLITE_ERROR_END();

    return res;
}

sqlite_err_t create_table(sqlite3 ** db, char * database_name, char * table_name, const char * format) {
    sqlite_err_t res = SQLITE_OK;
    if (*db == NULL) {
        res = open_db(db, database_name);
    }
    SQLITE_ERROR_START(res);
    return res;
    SQLITE_ERROR_END();

    size_t alloc_size;
    char * sql_create = malloc((alloc_size = (COMMAND_LIMIT) * sizeof(*sql_create)));
    memset(sql_create, 0, alloc_size);
    sprintf(sql_create, "CREATE TABLE IF NOT EXISTS %s %s", table_name, format);

    res = exec_task(db, sql_create);
    free(sql_create);
    SQLITE_ERROR_HANDLE(res);

    return res;
}

sqlite_err_t add_document(sqlite3 ** db, char * location, 
                                         char * grid_name, 
                                         char * id) {
    sqlite_err_t res = SQLITE_OK;

    res = create_table(db, location, grid_name, "(Id INTEGER PRIMARY KEY, name TEXT);");
    SQLITE_ERROR_START(res);
    return res;
    SQLITE_ERROR_END();

    size_t alloc_size;
    char request_buffer[] = "INSERT INTO %s (name) VALUES('%s');";
    int command_count = strlen(request_buffer) + strlen(grid_name) + strlen(id);
    char * sql_insert = malloc( (alloc_size = (command_count * sizeof(*sql_insert))) );
    if (sql_insert != NULL) {
        memset(sql_insert, 0, alloc_size);
        sprintf(sql_insert, request_buffer, grid_name, id);

        res = exec_task(db, sql_insert);
        free(sql_insert);
    }
    SQLITE_ERROR_HANDLE(res);

    return res;
}

sqlite_err_t add_section(sqlite3 ** db, char * database_name, char * name) {
    sqlite_err_t res = SQLITE_OK;

    res = create_table(db, database_name, SECTION, "(Id INTEGER PRIMARY KEY, section TEXT, location TEXT);");
    SQLITE_ERROR_START(res);
    printf("error creating table");
    return res;
    SQLITE_ERROR_END();

    size_t alloc_size;
    char * sql_insert = malloc( (alloc_size = (COMMAND_LIMIT * sizeof(*sql_insert))) );
    memset(sql_insert, 0, alloc_size);
    sprintf(sql_insert, "INSERT INTO %s (section, location) VALUES('%s', '%s');", SECTION, name, name);

    res = exec_task(db, sql_insert);
    free(sql_insert);
    SQLITE_ERROR_HANDLE(res);

    return res;
}

sqlite_err_t add_project(sqlite3 ** db, char * database_name, char * project) {
    sqlite_err_t res = SQLITE_OK;

    res = create_table(db, database_name, PROJECT, "(Id INTEGER PRIMARY KEY, project TEXT);");
    SQLITE_ERROR_START(res);
    printf("error creating table");
    return res;
    SQLITE_ERROR_END();

    size_t alloc_size;
    char * sql_insert = malloc( (alloc_size = (COMMAND_LIMIT * sizeof(*sql_insert))) );
    memset(sql_insert, 0, alloc_size);
    sprintf(sql_insert, "INSERT INTO %s (project) VALUES('%s');", PROJECT, project);

    res = exec_task(db, sql_insert);
    free(sql_insert);
    SQLITE_ERROR_HANDLE(res);

    return res;
}

sqlite_err_t drop_table(sqlite3 ** db, char * table_name) {
    return 0;
}

int read_data(void * notused, int argc, char ** argv , char ** azColName) { 
    jobject_holder_t * h = notused; 
    if (h->type == DOCNUM) {
        
    }
    else if (h->type == SECNUM) {
        if (h->head == NULL) {
            h->head = create_jobject("section", LIST, (data_t) {.list = NULL});
        }
        char * str = malloc((strlen(argv[2] + 1) * sizeof(*str)));
        strcpy(str, argv[2]);
        jobject * temp = create_jobject("name", TEXT, (data_t) {.txt = str});
        append_jlist(&h->head->data.list, OBJ, (data_t) {.obj = temp});
    }
    else if (h->type == PROJNUM) {
        if (h->head == NULL) {
            h->head = create_jobject("project", LIST, (data_t) {.list = NULL});
        }
        char * static_name = strchr(argv[1], '/') + 1;
        char * name = malloc((strlen(static_name) + 1) * sizeof(*name));
        strcpy(name, static_name);
        
        size_t alloc_size = static_name - argv[1];
        char * group = malloc(alloc_size);
        memset(group, 0, alloc_size);
        strncpy(group, argv[1], alloc_size - sizeof(*group));

        jobject * temp = create_jobject("name", TEXT, (data_t) {.txt = name});
        append_jobject(&temp, "group", TEXT, (data_t) {.txt = group});

        printf("GET PROJECT LIST: %s %s", name, group);
        append_jlist(&h->head->data.list, OBJ, (data_t) {.obj = temp});
    }
    return 0;
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

sqlite_err_t parse_data(sqlite3 ** db, char * database_name, char * table_name, jobject ** a_head) { 
    sqlite_err_t res = SQLITE_OK;
    jobject_holder_t h;
    memset(&h, 0, sizeof(h));
    h.type = (0 == strcmp(table_name, DOCTABLE)) ? DOCNUM: 
             (0 == strcmp(table_name, SECTION))  ? SECNUM:
             (0 == strcmp(table_name, PROJECT))  ? PROJNUM: NONE;

    if (*db == NULL) {
        res = open_db(db, database_name);
    }

    size_t alloc_size;
    char * sql_read = malloc( (alloc_size = ((COMMAND_LIMIT) * sizeof(*sql_read))) );
    memset(sql_read, 0, sizeof(alloc_size));
    strcat(sql_read, "SELECT * FROM ");
    strcat(sql_read, table_name);

    char * error_msg = NULL;
    res = sqlite3_exec(*db, sql_read, read_data, &h, &error_msg);
    free(sql_read);
    SQLITE_ERROR_HANDLE(res);

    *a_head = h.head;
    return res;
}
