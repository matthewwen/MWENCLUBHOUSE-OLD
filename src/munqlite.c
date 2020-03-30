#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unqlite.h>
#include <assert.h>

#define USERDB "/home/mwen/data/user.db"

typedef struct{
	char * password;
}user_response_t;

int DataConsumerCallback1(const void *pOutput,unsigned int nOutputLen,void *pUserData){
	const char * constpassword = pOutput;
	user_response_t * r = pUserData;

	// has a previous password reading
	if (r->password != NULL) {
		free(r->password);
	}

	// update password
	size_t alloc_size = (strlen(constpassword) + 1) * sizeof(*r->password);
	r->password = malloc(alloc_size);
	if (r->password != NULL) {
		memset(r->password, 0, alloc_size);
		memcpy(r->password, pOutput, nOutputLen);
	}

	return UNQLITE_OK;
}

char * get_password(const char * username) {
	user_response_t r;
	memset(&r, 0, sizeof(r));
	if (username != NULL) {
		unqlite * pDB = NULL;
		unqlite_open(&pDB, USERDB, UNQLITE_OPEN_CREATE|UNQLITE_OPEN_READONLY);
		unqlite_kv_fetch_callback(pDB, username, -1, DataConsumerCallback1, &r);
		unqlite_close(pDB);
	}
	unqlite_lib_shutdown();
	return r.password;
}

bool set_password(const char * username, const char * password) {
	int rc = UNQLITE_OK; 
	char * data_password = get_password(username);
	if (data_password == NULL) {
		unqlite * pDB = NULL;
		rc = unqlite_open(&pDB, USERDB, UNQLITE_OPEN_CREATE|UNQLITE_OPEN_READWRITE);
		rc = unqlite_kv_append_fmt(pDB, username, -1, "%s", password);
		unqlite_close(pDB);
		unqlite_lib_shutdown();
	}
	else {
		free(data_password);
	}
	return rc == UNQLITE_OK;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
