#ifndef _OAUTH_H
#define _OAUTH_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum {OAUTH_SEC_UNKNOWN = -1, OAUTH_SEC_DEFAULT = 0, OAUTH_SEC_ADMIN = INT64_MAX} oauth_sec_t;

typedef enum {OAUTH_TOKEN, OAUTH_REFRESH} oauth_token_t;

char * oauth_get_hashed_pwd(const char * username, char ** a_error);
void   oauth_signup(char * username, char * pwd);
bool   oauth_isValid(char * username, char * password);
char * oauth_auth_token(const char * username, oauth_token_t type);
char * oauth_refresh_token(char * refresh_token);
bool   oauth_isValid_token(char * token, oauth_sec_t req_security);
void   oauth_update_security(char * token, char * username, oauth_sec_t sec);
void   oauth_init_admin();

#endif