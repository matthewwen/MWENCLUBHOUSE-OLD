#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "security.h"

static int DAY_IN_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void mwendate_tostring(mwentime_t t, char outputBuffer[16]) {
	sprintf(outputBuffer, "%02d%02d%04dT%02d%02d%02d", t.month, t.day, t.year, t.hour, t.min, t.sec);
}

mwentime_t get_time_from_buffer(const char * datestr) {
	int len = strlen(datestr);
	mwentime_t t;
	memset(&t, 0, sizeof(t));
	if (len == 15) {
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
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
