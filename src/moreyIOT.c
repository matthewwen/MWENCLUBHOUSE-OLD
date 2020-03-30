#include "Python.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "msqlite.h"
#include "moreyIOT.h"
#include "common.h"
#include "security.h"
#include "json.h"

#define LIGHTDATABASE "/home/mwen/data/light.db"

void light_handler(struct lws * wsi, bool * found, struct request * r);
void light_state_hanlder(struct lws * wsi, bool * found, struct request * r);

int state_sqlite3_handler(void * notused, int argc, char ** argv , char ** azColName) { 
	// declear variables
	jlist  ** a_list = notused;
	jlist   * list   = *a_list;
	jobject * obj    = NULL;

	if (argc < 3) {
		return 0;
	}

	// get the id
	int id = atoi(argv[0]);
	append_jobject(&obj, azColName[0], NUM, (data_t) {.num = id});

	//get the time
	char * str = malloc((strlen(argv[1]) + 1) * sizeof(*str));
	strcpy(str, argv[1]);
	append_jobject(&obj, azColName[1], TEXT, (data_t) {.txt = str});

	// get the state
	int state = atoi(argv[2]);
	char * statestr = state == 1 ? "on": "off";
	append_jobject(&obj, azColName[2], TEXT_STATIC, (data_t) {.txt = statestr});

	if (obj != NULL) {
		append_jlist(&list, OBJ, (data_t) {.obj = obj});
	}
	*a_list = list;
	return 0;
}

jobject * get_light() {
	sqlite3 * db = NULL;
	create_table(&db, LIGHTDATABASE, "STATE", "(Id INTEGER PRIMARY KEY, date INTEGER, state INTEGER)");
	jlist * list = NULL;

	if (db != NULL) {
		sqlite3_exec(db, "SELECT * FROM STATE;", state_sqlite3_handler, &list, NULL);
	}

	if (db != NULL) {sqlite3_close(db);}

	return list != NULL ? create_jobject("data", LIST, (data_t) {.list = list}): NULL;
}

void morey_handler(const char * url, struct lws * wsi, bool * found, struct request * r) {
	url = url + strlen(MOREYIOT);
	if (strcmp(url, "/light") == 0) {
		light_handler(wsi, found, r);
	}
	else if (strcmp(url, "/lightstates") == 0) {
		light_state_hanlder(wsi, found, r);
	}
}

void light_state_hanlder(struct lws * wsi, bool * found, struct request * r) {
#ifdef ALLOWPYTHON
	//PyRun_SimpleString("a = pub.publish(a, \"update\")");
#endif
	jobject * jlightstate = get_light();
	if (jlightstate != NULL) {
		json_tostring(&r->buff, jlightstate, &r->alloc_size);
		free_json(&jlightstate);
		if ((*found = (r->buff != NULL))) {
			CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
		}
	}
}

void light_handler(struct lws * wsi, bool * found, struct request * r) {
	jobject * robj = user_auth(wsi);
	jobject * temp = get_jobject("canEdit", robj);
	bool is_valid = temp != NULL && temp->data.cond; // change later

	char * url_arg = NULL, * servicestr = NULL;
	if (is_valid) {
		url_arg    = get_header_data(wsi, WSI_TOKEN_HTTP_URI_ARGS);
		servicestr = get_url_arg(url_arg, "service=");
	}
	if (url_arg != NULL) {free(url_arg);}

	int service = -1;
	if (servicestr != NULL) {
		service = atoi(servicestr);
	}

	char * jsonservice = "unknown";
	if (service == 1) {
		jsonservice = "aws";
#ifdef ALLOWPYTHON
		char * pythoncode = "if running == 0:\n"
			"\trunning = 1\n"
			"\ta = pub.publish(a, \"light\")\n"
			"\trunning = 0\n";
		PyRun_SimpleString(pythoncode);
#endif
	}
	else if (service == 2) {
		jsonservice = "azure";
	}
	else if (service == 3) {
		jsonservice = "google cloud";
	}

	if (servicestr != NULL) {
		append_jobject(&robj, "service", TEXT_STATIC, (data_t) {.txt = jsonservice});
		free(servicestr);
	}

	json_tostring(&r->buff, robj, &r->alloc_size);
	free_json(&robj);
	if ((*found = (r->buff != NULL))) {
		CREATE_REQUEST(r, BUFFER_REQUEST, r->alloc_size - 1, r->buff, BUFFER_MALLOC);
	}
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
