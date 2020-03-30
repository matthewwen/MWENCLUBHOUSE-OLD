#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "todo.h"
#include "msqlite.h"
#include "security.h"
#include "json.h"
#include "common.h"

#define TASKDATABASE "/home/mwen/data/todo.db"

#define TODOSECTION "/section"
#define TASK    "/task"
#define CAL     "/calendar"

#define CREATE "/create"
#define DELETE "/delete"
#define COMPLETE "/complete"

#define TASK_COL "(Id INTEGER PRIMARY KEY, name TEXT, duedate INTEGER, complete INTEGER, section INTEGER)"

void section_handler(const char * url, struct lws * wsi, bool * found, struct request * r);
void task_handler(const char * url, struct lws * wsi, bool * found, struct request * r);
void cal_handler(const char * url, struct lws * wsi, bool * found, struct request * r);

// section additions
void get_section(const char * url, struct lws * wsi, bool * found, struct request * r);
void create_section(const char * url, struct lws * wsi, bool * found, struct request * r);

// tasks additons
void create_task(const char * url, struct lws * wsi, bool * found, struct request * r);
void get_task(const char * url, struct lws * wsi, bool * found, struct request * r);
void complete_task(const char * url, struct lws * wsi, bool * found, struct request * r);

void todo_handler(const char * url, struct lws * wsi, bool * found, struct request * r) {
	url = url + strlen(TODOURL);
	if (strncmp(url, TODOSECTION, strlen(TODOSECTION)) == 0) {
		section_handler(url, wsi, found, r);
	}
	else if (strncmp(url, TASK, strlen(TASK)) == 0) {
		task_handler(url, wsi, found, r);
	}
	else if (strncmp(url, CAL, strlen(CAL)) == 0) {
		cal_handler(url, wsi, found, r);
	}
}

void section_handler(const char * url, struct lws * wsi, bool * found, struct request * r) {
	url = url + strlen(TODOSECTION);
	if ((strcmp(url, "/") == 0) || *url == '\0') {
		get_section(url, wsi, found, r);
	}
	else if (strncmp(url, CREATE, strlen(CREATE)) == 0) {
		create_section(url, wsi, found, r);
	}
	else if (strncmp(url, DELETE, strlen(DELETE)) == 0) {

	}
}

void task_handler(const char * url, struct lws * wsi, bool * found, struct request * r) {
	url = url + strlen(TASK);
	if ((strcmp(url, "/") == 0) || *url == '\0') {
		get_task(url, wsi, found, r);
	}
	else if (strncmp(url, CREATE, strlen(CREATE)) == 0) {
		create_task(url, wsi, found, r);
	}
	else if (strncmp(url, DELETE, strlen(DELETE)) == 0) {
	}
	else if (strncmp(url, COMPLETE, strlen(COMPLETE)) == 0) {
		complete_task(url, wsi, found, r);
	}
	
}
void cal_handler(const char * url, struct lws * wsi, bool * found, struct request * r){ 
	url = url + strlen(CAL);
	//printf("cal: %s\n", url);
}

///////////////////////////////////////////////////////////////////////////////////////////
int get_section_helper(void * notused, int argc, char ** argv , char ** azColName) { 
	// declear variables
	jlist  ** a_list = notused;
	jlist   * list   = *a_list;
	jobject * obj    = NULL;

	if (argc < 4) {
		return 0;
	}

	// get the id
	long id = atoi(argv[0]);
	append_jobject(&obj, azColName[0], NUM, (data_t) {.num = id});

	//get the time
	char * name = malloc((strlen(argv[1]) + 1) * sizeof(*name));
	if (name != NULL) {
		strcpy(name, argv[1]);
		append_jobject(&obj, azColName[1], TEXT, (data_t) {.txt = name});
	}

	// get the duedate
	long state = atoi(argv[2]);
	append_jobject(&obj, azColName[2], NUM, (data_t) {.num = state});

	// get the complete
	long comp = atoi(argv[3]);
	append_jobject(&obj, azColName[3], NUM, (data_t) {.num = comp});

	if (obj != NULL) {
		append_jlist(&list, OBJ, (data_t) {.obj = obj});
	}
	*a_list = list;
	return 0;
}

void get_section(const char * url, struct lws * wsi, bool * found, struct request * r) {
	jobject * auth = admin_auth(wsi);
	jobject * canEdit = get_jobject("canEdit", auth);
	bool is_valid = canEdit != NULL && canEdit->type == CON && canEdit->data.cond;

	sqlite3 * db = NULL;
	if (is_valid) {
		create_table(&db, TASKDATABASE, "SECTION", "(Id INTEGER PRIMARY KEY, name TEXT, "
				"duedate INTEGER, complete INTEGER)");
		is_valid = (db != NULL);
	}	

	jlist * list = NULL;
	if (is_valid) {
		sqlite3_exec(db, "SELECT * FROM SECTION;", get_section_helper, &list, NULL);
	}
	if (db != NULL) {sqlite3_close(db);}

	if (list != NULL) {
		append_jobject(&auth, "data", LIST, (data_t) {.list = list});
	}
	json_tostring(&r->buff, auth, &r->alloc_size);
	free_json(&auth);
	if ((*found = (r->buff != NULL))) {
		CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
	}
}

void create_section(const char * url, struct lws * wsi, bool * found, struct request * r) {
	jobject * auth = admin_auth(wsi);
	jobject * canEdit = get_jobject("canEdit", auth);
	bool is_valid = canEdit != NULL && canEdit->type == CON && canEdit->data.cond;

	char * url_arg = NULL, * name = NULL, * datestr = NULL;
	if (is_valid) {
		url_arg = get_header_data(wsi, WSI_TOKEN_HTTP_URI_ARGS);
		name    = get_url_arg(url_arg, "name=");
		datestr = get_url_arg(url_arg, "date=");
		if (url_arg != NULL) {free(url_arg);}
	}

	long date = datestr == NULL ? -1: atoi(datestr);
	if (datestr != NULL) {free(datestr);}

	if (name != NULL) {
		sqlite3 * db = NULL;	
		create_table(&db, TASKDATABASE, "SECTION", "(Id INTEGER PRIMARY KEY, name TEXT, "
				"duedate INTEGER, complete INTEGER)");
		char * format = "INSERT INTO SECTION (name, duedate, complete) VALUES (\"%s\", %ld, -1);";
		int len = strlen(format) - 5 + strlen(name);
		if (date <= 0) {
			len += 1;
		}
		for (long cpdate = (date < 0) ? -1 * date: date; cpdate > 0; cpdate /= 10) {len++;}
		char * buffer = malloc((len + 1) * sizeof(*buffer));
		if ((is_valid = (buffer != NULL))) {
			memset(buffer, 0, (len + 1) * sizeof(*buffer));
			sprintf(buffer, format, name, date);
			exec_task(&db, buffer);
			free(buffer);
		}
		free(name);
		sqlite3_close(db);
		append_jobject(&auth, "success", CON, (data_t) {.cond = is_valid});
	}
	json_tostring(&r->buff, auth, &r->alloc_size);
	free_json(&auth);
	if ((*found = (r->buff != NULL))) {
		CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
int get_task_helper(void * notused, int argc, char ** argv , char ** azColName) { 
	// declear variables
	jlist  ** a_list = notused;
	jlist   * list   = *a_list;
	jobject * obj    = NULL;

	if (argc < 5) {
		return 0;
	}

	// get the id
	long id = atoi(argv[0]);
	append_jobject(&obj, azColName[0], NUM, (data_t) {.num = id});

	//get the name
	char * name = malloc((strlen(argv[1]) + 1) * sizeof(*name));
	if (name != NULL) {
		strcpy(name, argv[1]);
		append_jobject(&obj, azColName[1], TEXT, (data_t) {.txt = name});
	}

	// get the duedate
	long state = atoi(argv[2]);
	append_jobject(&obj, azColName[2], NUM, (data_t) {.num = state});

	// get the complete
	long comp = atoi(argv[3]);
	append_jobject(&obj, azColName[3], NUM, (data_t) {.num = comp});

	// get the section
	long section = atoi(argv[4]);
	append_jobject(&obj, azColName[4], NUM, (data_t) {.num = section});

	if (obj != NULL) {
		append_jlist(&list, OBJ, (data_t) {.obj = obj});
	}
	*a_list = list;
	return 0;
}

void get_task(const char * url, struct lws * wsi, bool * found, struct request * r) {
	jobject * auth = admin_auth(wsi);
	jobject * canEdit = get_jobject("canEdit", auth);
	bool is_valid = canEdit != NULL && canEdit->type == CON && canEdit->data.cond;

	long id = 0;
	char * get_all = NULL;
	if (is_valid) {
		char * url_arg = get_header_data(wsi, WSI_TOKEN_HTTP_URI_ARGS);
		char * id_str  = get_url_arg(url_arg, "id=");
		get_all        = get_url_arg(url_arg, "query=");
		if (url_arg != NULL) {free(url_arg);}
		
		if (id_str != NULL) {
			id = atol(id_str);
			free(id_str);
		}
		id = id < 0 ? 0: id;
	}

	int size = id == 0 ? 1: 0;
	char * buffer = NULL;
	if (is_valid) {
		char * format = "SELECT * FROM TASK WHERE section=%ld;";
		if (get_all != NULL && strcmp(get_all, "uncompleted") == 0) {
			format = "SELECT * FROM TASK WHERE section=%ld AND COMPLETE=-1;";
		}
		size          += strlen(format) - 3;
		for (long cpid = id; cpid > 0; cpid /= 10) {size++;}
		buffer = malloc((size + 1) * sizeof(*buffer));
		if (buffer != NULL) {
			sprintf(buffer, format, id);
		}
	}
	if (get_all != NULL) {free(get_all);}

	jlist * data = NULL;
	if (buffer != NULL) {
		sqlite3 * db = NULL;
		create_table(&db, TASKDATABASE, "TASK", "(Id INTEGER PRIMARY KEY,"
				" name TEXT, duedate INTEGER, complete INTEGER, section INTEGER)");
		sqlite3_exec(db, buffer, get_task_helper, &data, NULL);
		free(buffer);
		if (db != NULL) {sqlite3_close(db);}
	}

	append_jobject(&auth, "data", LIST, (data_t) {.list = data});
	append_jobject(&auth, "success", CON, (data_t) {.cond = is_valid});
	json_tostring(&r->buff, auth, &r->alloc_size);
	free_json(&auth);
	if ((*found = (r->buff != NULL))) {
		CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
	}
}

void create_task(const char * url, struct lws * wsi, bool * found, struct request * r) {
	jobject * auth = admin_auth(wsi);
	jobject * canEdit = get_jobject("canEdit", auth);
	bool is_valid = canEdit != NULL && canEdit->type == CON && canEdit->data.cond;

	char * name = NULL, * duedate = NULL, * section_id = NULL;
	if (is_valid) {
		char * url_arg = get_header_data(wsi, WSI_TOKEN_HTTP_URI_ARGS);
		name           = get_url_arg(url_arg, "name=");
		duedate        = get_url_arg(url_arg, "duedate=");
		section_id     = get_url_arg(url_arg, "section=");
		if (url_arg != NULL) {free(url_arg);};
		is_valid = (name != NULL) && (section_id != NULL);
	}

	int size = 0;
	long duedate_long, section_long;
	if (is_valid) {
		duedate_long = duedate != NULL ? atol(duedate): -1;
		section_long = atol(section_id);

		long current_time    = time(NULL);
		duedate_long = (duedate_long < current_time) ? current_time: duedate_long;

		if ((is_valid = (section_long >= 0))) {
			size += duedate_long == -1 ? 2: duedate_long == 0 ? 1: 0;
			size += section_long == 0  ? 1: 0;

			for (long cp_duedate_long = duedate_long; 
					cp_duedate_long > 0; cp_duedate_long /= 10) {size++;}
			for (long cp_section_long = section_long; 
					cp_section_long > 0; cp_section_long /= 10) {size++;}

		}
	}
	if (duedate    != NULL) {free(duedate);}
	if (section_id != NULL) {free(section_id);}

	sqlite3 * db = NULL;		
	if (is_valid) {
		create_table(&db, TASKDATABASE, "TASK", TASK_COL);
		is_valid = (db != NULL);
	}
	
	if (is_valid) {
		char * format = "INSERT INTO TASK (name, duedate, complete, section) "
		                "VALUES(\"%s\", %ld, -1, %ld);";
		size += strlen(format) - 8 + strlen(name);
		char * buffer = malloc((size + 1) * sizeof(*buffer));
		if (buffer != NULL) {
			memset(buffer, 0, (size + 1) * sizeof(*buffer));
			sprintf(buffer, format, name, duedate_long, section_long);
			exec_task(&db, buffer);
			free(buffer);
		}
		sqlite3_close(db);
	}
	if (name != NULL) {free(name);}

	append_jobject(&auth, "success", CON, (data_t) {.cond = is_valid});
	json_tostring(&r->buff, auth, &r->alloc_size);
	free_json(&auth);
	if ((*found = (r->buff != NULL))) {
		CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
	}
}

void complete_task(const char * url, struct lws * wsi, bool * found, struct request * r) {
	jobject * auth = admin_auth(wsi);
	jobject * canEdit = get_jobject("canEdit", auth);
	bool is_valid = canEdit != NULL && canEdit->type == CON && canEdit->data.cond;

	long id = -1;
	long section = -1;
	if (is_valid) {
		char * url_arg = get_header_data(wsi, WSI_TOKEN_HTTP_URI_ARGS);
		char *  id_str = get_url_arg(url_arg, "id=");
		char * sec_str = get_url_arg(url_arg, "section=");
		if (url_arg != NULL) {free(url_arg);}
		id      = id_str != NULL ? atol(id_str): -1;
		section = sec_str != NULL ? atol(sec_str): -1;
		if (id_str  != NULL) {free(id_str);}
		if (sec_str != NULL) {free(sec_str);}
	}

	long t = time(NULL);
	sqlite3 * db = NULL;
	if ((is_valid = (id >= 0 && t > 0 && section >= 0))) {
		create_table(&db, TASKDATABASE, "TASK", TASK_COL);
	}

	if ((is_valid = (db != NULL))) {
		char * format = "UPDATE TASK SET complete=%ld WHERE Id=%ld AND section=%ld;";
		int len = strlen(format) - 9 + (t == 0 ? 1: 0) + (id == 0 ? 1: 0) + (section == 0 ? 1: 0);
		for (long cptime = t; cptime > 0; cptime /= 10) {len++;}
		for (long cpid   = id;   cpid > 0;   cpid   /= 10) {len++;}
		for (long cpsec  = section; cpsec > 0; cpsec /= 10) {len++;}
		char * buffer = malloc( (len + 1) * sizeof(*buffer) );
		if (buffer != NULL) {
			sprintf(buffer, format, time, id, section);
			exec_task(&db, buffer);
			free(buffer);
		}
	}
	if (db != NULL) {sqlite3_close(db);}

	append_jobject(&auth, "success", CON, (data_t) {.cond = is_valid});
	json_tostring(&r->buff, auth, &r->alloc_size);
	free_json(&auth);
	if ((*found = (r->buff != NULL))) {
		CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
	}
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
