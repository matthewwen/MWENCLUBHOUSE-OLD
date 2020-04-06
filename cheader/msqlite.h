#ifndef _MSQLITE_H_
#define _MSQLITE_H_
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdbool.h>
#include "json.h"

typedef uint8_t sqlite_err_t;

#define DOCTABLE "documents"
#define SECTION "section"
#define PROJECT "project"

//enum {NONE, DOCNUM, SECNUM, PROJNUM, PAGE, IMG, OTHER};

#define SQLITE_ERROR_START(res)\
    if (res) {\
        printf("error: %d\n", res);

#define SQLITE_ERROR_END() \
    }

#define SQLITE_ERROR_HANDLE(res)  \
        SQLITE_ERROR_START(res);\
        SQLITE_ERROR_END();\

sqlite_err_t open_db(sqlite3 **, char *);
sqlite_err_t add_document(sqlite3 **, char *, char *, char *);
sqlite_err_t create_table(sqlite3 **, char *, char *, const char *);
sqlite_err_t exec_task(sqlite3 **, char *);
sqlite_err_t drop_table(sqlite3 **, char *);

// personal function
sqlite_err_t parse_data(sqlite3 ** db, char * database_name, char * table_name, jobject ** a_head);


#endif
