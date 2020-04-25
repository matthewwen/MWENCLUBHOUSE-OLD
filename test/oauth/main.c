#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "oauth.h"

START_TEST (get_hash_null) {
	char * error_msg = NULL;
	char * hash = oauth_get_hashed_pwd(NULL, &error_msg);
	ck_assert(hash == NULL);
	ck_assert(error_msg == NULL);

	hash = oauth_get_hashed_pwd(NULL, NULL);
	ck_assert(hash == NULL);
}
END_TEST

START_TEST (user_not_exist) {
	oauth_signup("user_not_exist", "test0");
	char * error_msg = NULL;
	char * hash = oauth_get_hashed_pwd("0user_not_exist", &error_msg);
	ck_assert(hash == NULL);
	ck_assert(error_msg == NULL);

	// check if user not exist exists
	hash = oauth_get_hashed_pwd("user_not_exist", &error_msg);
	ck_assert(hash != NULL);
	ck_assert(error_msg == NULL);

	free(hash);
}
END_TEST

START_TEST (signup_error) {
	// success to signup
	oauth_signup("signup_error_1", "pwd1");
	bool signup = oauth_isValid("signup_error_1", "pwd1");
	ck_assert(signup == true);

	// fail to signup
	oauth_signup("signup_error_1", "pwd2");
	signup = oauth_isValid("signup_error_1", "pwd2");
	ck_assert(signup == false);
	signup = oauth_isValid("signup_error_1", "pwd1");
	ck_assert(signup == true);
}
END_TEST

START_TEST (signup_null) {
	char * error_msg = NULL;
	// fail to signup user, both is null
	oauth_signup(NULL, NULL);

	// fail to signup, both is null
	oauth_signup("signup_error_0", NULL);
	char * hash = oauth_get_hashed_pwd("signup_error_0", &error_msg);
	ck_assert(hash == NULL);
	ck_assert(error_msg == NULL);

	// fail to signup, no username
	oauth_signup(NULL, "pwd");
}
END_TEST

START_TEST (isValid_error) {
	bool isValid = false;

	isValid = oauth_isValid(NULL, NULL);
	ck_assert(isValid == false);

	oauth_signup("isValid", "isValid");
	isValid = oauth_isValid("isValid", NULL);
	ck_assert(isValid == false);

	isValid = oauth_isValid(NULL, "isValid");
	ck_assert(isValid == false);

	isValid = oauth_isValid("isValid", "isValid");
	ck_assert(isValid == true);

	oauth_signup("isValid", "notIsValid");
	isValid = oauth_isValid("isValid", NULL);
	ck_assert(isValid == false);

	oauth_signup("isValid", "notIsValid");
	isValid = oauth_isValid("isValid", "notIsValid");
	ck_assert(isValid == false);

	isValid = oauth_isValid("isValid", "isValid");
	ck_assert(isValid == true);
}
END_TEST

START_TEST (valid_token) {
	bool isValid = false; 

	// nothing being passed
	isValid = oauth_isValid_token(NULL, OAUTH_SEC_DEFAULT);
	ck_assert(isValid == false);

	// expired token
	char * token_expired   = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE1ODc2MDkwMjEsInR5cGUiOiJ0b2tlbiIsInVzZXIiOiJ0b2tlbjAifQ.LyNfT700YKEtY-jswem4fKxwGJfKM6uwLZm3ptG_I5A";
	isValid = oauth_isValid_token(token_expired, OAUTH_SEC_DEFAULT);
	ck_assert(isValid == false);

	// expired refresh token
	char * refresh_expired = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE1Nzk4MzQ5MjgsInR5cGUiOiJyZWZyZXNoIiwidXNlciI6InRva2VuMCJ9.Vw02KMxESFFW4R_5zw0fldO46F7B4h2gDGWOBkkGnvE";
	char * new_token = NULL;
	new_token = oauth_refresh_token(refresh_expired);
	ck_assert(new_token == NULL);

	oauth_signup("valid token", "vt");
	char * token = NULL;
	token = oauth_auth_token("valid token 0", OAUTH_TOKEN);
	ck_assert(token == NULL);
	token = oauth_auth_token("valid token", OAUTH_TOKEN);
	ck_assert(token != NULL);
	isValid = oauth_isValid_token(token, OAUTH_SEC_DEFAULT);
	free(token);
	ck_assert(isValid == true);

	char * rtoken = NULL; 
	rtoken = oauth_auth_token("valid token 0", OAUTH_REFRESH);
	ck_assert(rtoken == NULL);
	token = oauth_auth_token("valid token", OAUTH_TOKEN);
	ck_assert(token != NULL);
	free(token);
	rtoken = oauth_auth_token("valid token", OAUTH_REFRESH);
	ck_assert(rtoken != NULL);
	token = oauth_refresh_token(rtoken);
	ck_assert(token != NULL);
	isValid = oauth_isValid_token(token, OAUTH_SEC_DEFAULT);
	ck_assert(isValid == true);
	free(token);
	free(rtoken);
}
END_TEST;

START_TEST (security_test) {
	oauth_init_admin();

	char * token = NULL;
	bool isValid = false;
	token = oauth_auth_token("mwen", OAUTH_TOKEN);
	ck_assert(token != NULL);
	isValid = oauth_isValid_token(token, OAUTH_SEC_DEFAULT);
	ck_assert(isValid);
	isValid = oauth_isValid_token(token, OAUTH_SEC_ADMIN);
	ck_assert(isValid);
	free(token);

	// create fake user
	oauth_signup("security_test0", "pwd");
	token = oauth_auth_token("security_test0", OAUTH_TOKEN);
	isValid = oauth_isValid_token(token, OAUTH_SEC_DEFAULT);
	ck_assert(isValid);
	isValid = oauth_isValid_token(token, OAUTH_SEC_ADMIN);
	ck_assert(isValid == false);
	free(token);

	token = oauth_auth_token("mwen", OAUTH_TOKEN);
	oauth_update_security(token, "security_test0", OAUTH_SEC_ADMIN);
	free(token);

	token = oauth_auth_token("security_test0", OAUTH_TOKEN);
	isValid = oauth_isValid_token(token, OAUTH_SEC_ADMIN);
	ck_assert(isValid);
	free(token);
}
END_TEST;

Suite * oauth_suite(void) {
	Suite * s = suite_create("oauth");
	TCase * tc_core = tcase_create("Core");

	tcase_add_test(tc_core, get_hash_null);
	tcase_add_test(tc_core, user_not_exist);
	tcase_add_test(tc_core, signup_null);
	tcase_add_test(tc_core, signup_error);
	tcase_add_test(tc_core, isValid_error);
	tcase_add_test(tc_core, valid_token);
	tcase_add_test(tc_core, security_test);
	suite_add_tcase(s, tc_core);

	return s;
}

int main(int argc, char* argv[]) {
	remove("user.db");
	Suite * s = oauth_suite();
	SRunner * sr = srunner_create(s);
	srunner_set_fork_status(sr, CK_NOFORK);
	srunner_run_all(sr, CK_NORMAL);
	srunner_ntests_failed(sr);
	srunner_free(sr);

	return EXIT_SUCCESS;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
