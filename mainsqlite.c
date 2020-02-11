#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "msqlite.h"
#include "webdatabase.h"
#include "json.h"

int read_data_lol(void * notused, int argc, char ** argv , char ** azColName) { 
	jlist  ** a_list = notused;
	jlist   * list = *a_list;
	jobject * obj = NULL;

	int var_database = atoi(argv[2]);
	bool is_visible = var_database != 0;
	append_jobject(&obj, "visible", CON, (data_t) {.cond = is_visible});

	int count = strlen(argv[1]) + 1;
	char * str = malloc(count * sizeof(*str));
	strcpy(str, argv[1]);
	append_jobject(&obj, "href", TEXT, (data_t) {.txt = str});
	append_jlist(&list, OBJ, (data_t) {.obj = obj});

	*a_list = list;

	return 0;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		return EXIT_FAILURE;
	}
	if (strcmp(argv[1], "-h") == 0) {
		jlist * list = NULL;
		sqlite3 * db = NULL;
		get_web_record_href(&db);
		sqlite3_exec(db, "SELECT * FROM PAGEVIEW", read_data_lol, &list, NULL);
		assert(list != NULL);

		for (jlist * curr = list; curr != NULL; curr = curr->next) {
		}
		
		char * str = NULL;
		jobject * obj = create_jobject("data", LIST, (data_t) {.list = list});
		json_tostring(&str, obj, NULL);
		free(str);
		free_json(&obj);
    	if (db != NULL) {sqlite3_close(db); db = NULL;}
	}
	else if (strcmp(argv[1], "-i") == 0) {
		printf("it is for the images\n");
	}
	else if (strcmp(argv[1], "-g") == 0) {
		printf("it is for the grid\n");
	}

	return EXIT_SUCCESS;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
