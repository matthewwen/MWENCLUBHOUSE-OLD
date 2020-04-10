#ifndef _EXPO_SCHOOL_H
#define _EXPO_SCHOOL_H

#include <stdio.h>
#include <stdlib.h>

typedef struct exp_lstring_t{
	char * msg;
	char * href;
}exp_lstring_t;

typedef struct exp_index_t{
	int * idx;
	int len;
}exp_index_t;

typedef struct exp_school_t{
	int id;
	char * name;
	bool online;
	int pop;
	int href_len;
	exp_lstring_t * href;		
}exp_school_t;

void init_exposchool(char * str);

#endif