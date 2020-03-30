#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libwebsockets.h>
#include "security.h"
#include "common.h"
#include "json.h"
#include "munqlite.h"

static int DAY_IN_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void mwendate_tostring(mwentime_t t, char * outputBuffer) {
	memset(outputBuffer, 0, 16);
	// month
	outputBuffer[0] = '0' + (t.month >= 10 ? t.month / 10: 0);
	outputBuffer[1] = '0' + (t.month % 10);
	// day
	outputBuffer[2] = '0' + (t.day >= 10 ? t.day / 10: 0);
	outputBuffer[3] = '0' + (t.day % 10);
	// year
	outputBuffer[4] = '0' + (t.year > 1000 ? t.year / 1000: 0); 
	outputBuffer[5] = '0' + (t.year > 100 ? t.year / 100 % 10: 0); 
	outputBuffer[6] = '0' + (t.year > 10 ? t.year / 10 % 10: 0); 
	outputBuffer[7] = '0' + (t.year % 10); 

	outputBuffer[8] = 'T';

	// hour
	outputBuffer[9] = '0' + (t.hour >= 10 ? t.hour / 10: 0);
	outputBuffer[10] = '0' + (t.hour % 10);
	// min
	outputBuffer[11] = '0' + (t.min >= 10 ? t.min / 10: 0);
	outputBuffer[12] = '0' + (t.min % 10);
	//sec
	outputBuffer[13] = '0' + (t.sec >= 10 ? t.sec / 10: 0);
	outputBuffer[14] = '0' + (t.sec % 10);
}

mwentime_t get_time_from_buffer(const char * datestr) {
	mwentime_t t;
	memset(&t, 0, sizeof(t));
	if ( (datestr != NULL ? strlen(datestr): 0) == 15) {
		char buffer[5];
		memset(buffer, 0, sizeof(buffer));
		strncpy(buffer, datestr +  0, 2);
		t.month = atoi(buffer);

		strncpy(buffer, datestr +  2, 2);
		t.day = atoi(buffer);

		strncpy(buffer, datestr +  9, 2);
		t.hour = atoi(buffer);

		strncpy(buffer, datestr + 11, 2);
		t.min = atoi(buffer);

		strncpy(buffer, datestr + 13, 2);
		t.sec = atoi(buffer);

		strncpy(buffer, datestr +  4, 4);
		t.year = atoi(buffer);
	}
	return t;
}

long convert_time(mwentime_t t) {
	long min_sec  = 60;
	long hour_sec = 60 * min_sec;
	long day_sec  = 24 * hour_sec;
	long year_sec = 364 * day_sec;

	long value = t.sec + 
		t.min * min_sec + 
		t.hour * hour_sec + 
		t.day * day_sec + 
		(t.year - 2020) * year_sec;
	for (int i = 0; i < t.month - 1; i++) {
		value = value + DAY_IN_MONTH[i] * day_sec;
	}

	// leap years in the past
	int num_leap = (t.year - 2020) / 4;
	num_leap -= 1;
	value += (num_leap) * day_sec;

	// current leap year
	if (t.year % 4 == 0 && t.month > 2) {
		value += day_sec;
	}

	return value;
}

mwentime_t get_time() {
	mwentime_t t;
	memset(&t, 0, sizeof(t));
	time_t rawtime;
	time(&rawtime);
	struct tm * info = gmtime(&rawtime);
	t.year  = info->tm_year + 1900;
	t.month = info->tm_mon + 1;
	t.day   = info->tm_mday;
	t.hour  = info->tm_hour;
	t.min   = info->tm_min;
	t.sec   = info->tm_sec;
	return t;
}

void sha256_string(char * buffer, char outputBuffer[65]) {
	if (buffer == NULL) {
		return;
	}
	unsigned char hash[SHA256_DIGEST_LENGTH];
	memset(hash, 0, sizeof(hash));
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, buffer, strlen(buffer));
	SHA256_Final(hash, &sha256);
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
	}
	outputBuffer[64] = 0;
}

jobject * admin_auth(struct lws * wsi) {
	char * token = get_header_data(wsi, WSI_TOKEN_HTTP_AUTHORIZATION);
	char * date  = get_header_data(wsi, WSI_TOKEN_HTTP_EXPIRES); // edit where cookie is a json string instead

	long time_date = convert_time(get_time()) - convert_time(get_time_from_buffer(date));
	time_date      = time_date < 0 ? time_date * -1: time_date;
	do {
		mwentime_t t = get_time();
		char buffer[16];
		mwendate_tostring(t, buffer);
	}while(false);

	size_t alloc_size = 0;
	if (token != NULL && date != NULL) {
		bool canEdit = false;
		if (time_date >= 500) {
			free(date);
		}
		else {
			jobject * jobj = NULL;
			// get password
			char * password = "kushal";
			char * alloc_password = malloc((strlen(password) + 1) * sizeof(*alloc_password));
			strcpy(alloc_password, password);
			append_jobject(&jobj, "password", TEXT, (data_t) {.txt = alloc_password});
			append_jobject(&jobj, "time", TEXT, (data_t) {.txt = date});

			// get json string
			char * json_str = NULL;
			json_tostring(&json_str, jobj, &alloc_size);
			free_json(&jobj);

			// get token
			char tokenBuffer[65];
			memset(tokenBuffer, 0, sizeof(tokenBuffer));
			sha256_string(json_str, tokenBuffer);
			free(json_str);

			// compare token
			canEdit = (strcmp(tokenBuffer, token) == 0);
		}
		free(token);
		return create_jobject("canEdit", CON, (data_t) {.cond = canEdit});
	}
	if (token != NULL) {free(token);}
	if (date != NULL) {free(date);}
	return NULL;
}

jobject * user_auth(struct lws * wsi) {
	char * token    = get_header_data(wsi, WSI_TOKEN_HTTP_AUTHORIZATION);
	char * date     = get_header_data(wsi, WSI_TOKEN_HTTP_EXPIRES);
	char * urlarg   = get_header_data(wsi, WSI_TOKEN_HTTP_URI_ARGS);
	char * username = get_url_arg(urlarg, "username=");
	if (urlarg != NULL) {free(urlarg);}
	char * password = get_password(username);

	bool is_valid = (token != NULL) && (date != NULL) && 
		         (username != NULL) && (password != NULL);
	long time_date = convert_time(get_time()) - convert_time(get_time_from_buffer(date));
	time_date      = time_date < 0 ? time_date * -1: time_date;
	bool canEdit = false;
	if (is_valid) {
		// create json to token
		jobject * json_tokenstr = create_jobject("password", TEXT_STATIC, (data_t) {.txt = password});
		append_jobject(&json_tokenstr, "username", TEXT_STATIC, (data_t) {.txt = username});
		append_jobject(&json_tokenstr, "time", TEXT_STATIC, (data_t) {.txt = date});

		// get json string
		char * tokenstr = NULL;
		json_tostring(&tokenstr, json_tokenstr, NULL);
		free_json(&json_tokenstr);

		// get token
		char tokenBuffer[65];
		memset(tokenBuffer, 0, sizeof(tokenBuffer));
		sha256_string(tokenstr, tokenBuffer);
		free(tokenstr);

		canEdit = (strcmp(tokenBuffer, token) == 0);
	}

	if (token != NULL) {free(token);}
	if (date != NULL) {free(date);}
	if (username != NULL) {free(username);}

	return create_jobject("canEdit", CON, (data_t) {.cond = canEdit});
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
