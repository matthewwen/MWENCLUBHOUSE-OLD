#ifndef _MSQLITE_H_
#define _MSQLITE_H_
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t sqlite_err_t;

// typedef struct packet{
//     int    id; 
//     char * address;
//     struct packet * next; 
// }packet;

// typedef struct{
//     packet ** head;
//     packet ** tail;
// }list;

#define DOCTABLE "documents"
#define SECTION "section"
#define PROJECT "project"

/*
 * Description: If checking sqlite_err_t, This # define prints out the error. 
 *              This must end with an SQLITE_ERROR_END.
 */
#define SQLITE_ERROR_START(res)\
    if (res) {\
        printf("error: %d\n", res);

/*
 * Description: This closes an SQLITE_ERROR_START. Any action when it catches an 
 *              error goes before this # define.
 */
#define SQLITE_ERROR_END() \
    }

/*
 * Description: If There is no action that needs to be preformed. This just prints 
 *             out the error.
 */
#define SQLITE_ERROR_HANDLE(res)  \
        SQLITE_ERROR_START(res);\
        SQLITE_ERROR_END();\

sqlite_err_t open_db(sqlite3 ** db, char *);
sqlite_err_t create_table(sqlite3 ** db, char *, char * table_name, const char * format);
sqlite_err_t add_document(sqlite3 **, char *, char *, char *);
sqlite_err_t add_project(sqlite3 ** db, char * database_name, char * project);
sqlite_err_t add_section(sqlite3 ** db, char *, char * name);
sqlite_err_t exec_task(sqlite3 ** db, char * task);
sqlite_err_t parse_data(sqlite3 ** db, char * database_name, char * table_name, jobject ** a_head);

/*
 * Description: Drops the table if needed
 * Paramter: 
 * - db: address of the address of the database
 * - table_name: the table name of the database
 */
sqlite_err_t drop_table(sqlite3 ** db, char * table_name);


#endif
