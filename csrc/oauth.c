#include <stdio.h>
#include <stdlib.h>
#include <jwt.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <libgen.h>
#include <check.h>
#include <jansson.h>
#include <stdbool.h>
#include <sqlite3.h>
#include <math.h>
#include "oauth.h"

#define THIRTY_MIN (30 * 60)
#define NINTY_DAY  (90 * 24 * 60 * 60)
#define TOKEN   "token"
#define REFRESH "refresh"

unsigned char KEY256[] = "d07e52c88bd9129d5fa3872996c70da2f5d09a91903beee7ff53f7ade95914e2";

#define CREATE_TABLE "CREATE TABLE IF NOT EXISTS JWT_USER(id INTEGER PRIMARY KEY, username TEXT, hashedpwd TEXT, admin INTEGER);"

char * oauth_hash_pwd(char * username, char * pwd);
int oauth_callback(void * json, int argc, char * col_val[], char * col_name[]);
int oauth_security_type(void * json, int argc, char * col_val[], char * col_name[]);

int oauth_security_type(void * json, int argc, char * col_val[], char * col_name[]) {
	json_t * j = json;
	json_array_append_new(j, json_integer(atoll(col_val[0])));
	return 0;
}

int oauth_callback(void * json, int argc, char * col_val[], char * col_name[]) {
	json_t * j = json;
	json_array_append_new(j, json_string(col_val[0]));
	return 0;
}

char * oauth_get_hashed_pwd(const char * username, char ** a_error_msg) {
	sqlite3 * db = NULL;
	char * error_msg = NULL;
	const char * select_format = "SELECT (hashedpwd) FROM JWT_USER WHERE username=\"%s\";";
	char * output = NULL;

	if (username != NULL) {
		sqlite3_open("user.db", &db);
	}

	if (db != NULL) {
		sqlite3_exec(db, CREATE_TABLE, NULL, NULL, &error_msg);
	}

	json_t * j = NULL;
	int len    = 0; 
	char * select_str = NULL;
	if (db != NULL && error_msg == NULL) {
		j = json_array();
		len = strlen(username) + strlen(select_format);
		select_str = malloc(len);
	}

	if (select_str != NULL) {
		memset(select_str, 0, len);
		sprintf(select_str, select_format, username);
		sqlite3_exec(db, select_str, oauth_callback, j, NULL);
		free(select_str);
	}

	const char * const_output = NULL;
	if (json_is_array(j) && json_array_size(j) > 0) {
		json_t * joutput = json_array_get(j, 0);
		const_output = json_string_value(joutput);
		output = malloc(strlen(const_output) + 1);
	}

	if (const_output != NULL && output != NULL) {
		strcpy(output, const_output);
	}
	json_decref(j);
	sqlite3_close(db);
	
	if (a_error_msg != NULL) {
		*a_error_msg = error_msg;
	}

	return output;
}

void oauth_signup(char * username, char * pwd) {
	sqlite3 * db = NULL;
	char * error_msg = NULL;
	char * insert_format = "INSERT INTO JWT_USER(username, hashedpwd, admin) VALUES (\"%s\", \"%s\", 0);";

	char * stored_pwd = NULL, * hashed_pwd = NULL, * insert_str = NULL;
	if (username != NULL && pwd != NULL) {
		sqlite3_open("user.db", &db);
		stored_pwd = oauth_get_hashed_pwd(username, NULL);
		hashed_pwd = oauth_hash_pwd(username, pwd);
	}

	int len = 0;
	if (db != NULL && hashed_pwd != NULL && stored_pwd == NULL) {
		sqlite3_exec(db, CREATE_TABLE, NULL, NULL, &error_msg);
		len = strlen(insert_format) + strlen(username) + strlen(hashed_pwd) + 1;
		insert_str = malloc(len);
	}

	if (insert_str != NULL) {
		memset(insert_str, 0, len);
		sprintf(insert_str, insert_format, username, hashed_pwd);
		sqlite3_exec(db, insert_str, NULL, NULL, &error_msg);
		free(insert_str);
	}

	if (stored_pwd != NULL) {
		free(stored_pwd);
	}

	if (hashed_pwd != NULL) {
		free(hashed_pwd);
	}

	sqlite3_close(db);
	sqlite3_close(NULL);
}

char * oauth_hash_pwd(char * username, char * password) {
	char * output = NULL;
	jwt_t * jwt = NULL;
	if (username != NULL && password != NULL) {
		jwt_new(&jwt);
	}

	if (jwt != NULL) {
		jwt_add_grant(jwt, "user", username);
		jwt_add_grant(jwt, "pwd", password);
		jwt_set_alg(jwt, JWT_ALG_HS256, KEY256, sizeof(KEY256));
		output = jwt_encode_str(jwt);
		jwt_free(jwt);
	}

	return output;
}

bool oauth_isValid(char * username, char * password) {
	char * output     = oauth_hash_pwd(username, password);
	char * hashed_pwd = oauth_get_hashed_pwd(username, NULL);
	bool same = hashed_pwd == NULL  || output == NULL ? \
				false: strcmp(hashed_pwd, output) == 0;

	jwt_free_str(output);
	if (hashed_pwd != NULL) {free(hashed_pwd);}

	return same;
}


char * oauth_auth_token(const char * username, oauth_token_t type) {
	char * output = NULL;

	jwt_t * jwt = NULL;
	time_t expires = time(NULL) + (type == OAUTH_TOKEN ? THIRTY_MIN: NINTY_DAY);
	char * hashed_username = NULL;
	
	if (username != NULL) {
		jwt_new(&jwt);
		hashed_username = oauth_get_hashed_pwd(username, NULL);
	}

	if (jwt != NULL && hashed_username != NULL) {
		jwt_add_grant_int(jwt, "exp", expires);
		jwt_add_grant(jwt, "user", username);
		jwt_add_grant(jwt, "type", (type == OAUTH_TOKEN ? TOKEN: REFRESH));
		jwt_set_alg(jwt, JWT_ALG_HS256, KEY256, sizeof(KEY256));
		output = jwt_encode_str(jwt);
	}

	jwt_free(jwt);

	if (hashed_username != NULL) {
		free(hashed_username);
	}

	return output;
}

char * oauth_refresh_token(char * refresh_token) {
	char * output = NULL;
	jwt_t * jwt = NULL;

	if (refresh_token != NULL) {
		jwt_decode(&jwt, refresh_token, KEY256, sizeof(KEY256));
	}

	jwt_valid_t * jwt_valid = NULL;
	if (jwt != NULL) {
		jwt_valid_new(&jwt_valid, JWT_ALG_HS256);
	}

	if (jwt_valid != NULL) {
		jwt_valid_set_now(jwt_valid, time(NULL));
	}
	
	const char * username = NULL;
	const char * type     = NULL;
	bool isValid          = false;
	if (jwt_valid != NULL && jwt != NULL && 
		jwt_validate(jwt, jwt_valid) == JWT_VALIDATION_SUCCESS) {
		username = jwt_get_grant(jwt, "user");
		type     = jwt_get_grant(jwt, "type");
		isValid  = strcmp(type, REFRESH) == 0;
	}

	if (username != NULL && isValid) {
		output = oauth_auth_token(username, OAUTH_TOKEN);
	}

	jwt_free(jwt);
	jwt_valid_free(jwt_valid);

	return output;
}

bool oauth_isValid_token(char * token, oauth_sec_t req_security) {
	sqlite3 * db = NULL;
	jwt_t * jwt = NULL;

	bool isValid = false;

	if (token != NULL) {
		jwt_decode(&jwt, token, KEY256, sizeof(KEY256));
		sqlite3_open("user.db", &db);
		isValid = jwt != NULL && db != NULL;
	}

	jwt_valid_t * jwt_valid = NULL;
	if (isValid) {
		jwt_valid_new(&jwt_valid, JWT_ALG_HS256);
		isValid = jwt_valid != NULL;
	}

	if (isValid) {
		jwt_valid_set_now(jwt_valid, time(NULL));
		isValid = jwt_validate(jwt, jwt_valid) == JWT_VALIDATION_SUCCESS;
	}

	
	const char * type     = NULL;
	const char * username = NULL;
	if (isValid) {
		username = jwt_get_grant(jwt, "user");
		type     = jwt_get_grant(jwt, "type");
		isValid  = strcmp(type, TOKEN) == 0;
	}

	const char * select_format = "SELECT (admin) FROM JWT_USER WHERE username=\"%s\";";
	char * select_str = NULL, * error_msg = NULL;
	int len = 0;
	json_t * j = NULL;
	if (isValid) {
		len = strlen(username) + strlen(select_format);
		select_str = malloc(len);

		j = json_array();
		isValid = select_str != NULL && json_is_array(j);
	}

	oauth_sec_t t = OAUTH_SEC_UNKNOWN;
	if (isValid) {
		memset(select_str, 0, len);
		sprintf(select_str, select_format, username);
		sqlite3_exec(db, select_str, oauth_security_type, j, &error_msg);
		free(select_str);
		isValid = error_msg == NULL && json_array_size(j) > 0;
	}

	if (isValid) {
		json_t * joutput = json_array_get(j, 0);
		t = json_is_integer(joutput) ? json_integer_value(joutput): OAUTH_SEC_UNKNOWN;
		isValid = t >= req_security;
	}

	json_decref(j);
	jwt_free(jwt);
	jwt_valid_free(jwt_valid);
	sqlite3_close(db);

	return isValid;
}

void oauth_update_security(char * token, char * username, oauth_sec_t sec) {
	sqlite3     * db        = NULL;
	bool isValid = token != NULL && username != NULL;

	if (isValid) {
		sqlite3_open("user.db", &db);
		isValid = oauth_isValid_token(token, OAUTH_SEC_ADMIN);
	}

	const char * select_format = "UPDATE JWT_USER SET admin=%ld WHERE username=\"%s\";";
	int len = 0;
	char * update_str = NULL;
	if (isValid) {
		int nDigits = floor(log10(fabs(sec))) + 1;
		len = strlen(select_format) + strlen(username) + nDigits;
		update_str = malloc(len);
		isValid = (update_str != NULL);
	}

	if (isValid) {
		memset(update_str, 0, len);
		sprintf(update_str, select_format, sec, username);
		sqlite3_exec(db, update_str, NULL, NULL, NULL);
		free(update_str);
	}

	sqlite3_close(db);
}

void oauth_init_admin() {
	oauth_signup("mwen", "temp-pwd");
	// make me, Matthew Wen, as admin
	sqlite3 * db = NULL;
	sqlite3_open("user.db", &db);

	char * update_format = "UPDATE JWT_USER SET admin=%ld WHERE username=\"mwen\";";
	char long_buffer[500];
	if (db != NULL) {
		memset(long_buffer, 0, sizeof(long_buffer));
		sprintf(long_buffer, update_format, OAUTH_SEC_ADMIN);
		sqlite3_exec(db, long_buffer, NULL, NULL, NULL);
	}
	sqlite3_close(db);
}