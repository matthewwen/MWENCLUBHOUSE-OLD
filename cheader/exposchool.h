#ifndef _EXPO_SCHOOL_H
#define _EXPO_SCHOOL_H

#include <stdio.h>
#include <stdlib.h>

typedef struct exp_lstring_t{
	char * msg;
	char * href;
}exp_lstring_t;

typedef struct exp_school_t{
	int id;
	char * name;
	bool online;
	int pop;
	int href_len;
	exp_lstring_t * href;		
}exp_school_t;

typedef struct exp_lint_t{
	int value;
	struct exp_lint_t * next;
}exp_lint_t;

typedef struct exp_index_t{
	int * idx;
	int len;
	exp_lint_t * list;
	exp_lint_t ** list_end;
	int list_len;
}exp_index_t;

// void init_exposchool(char * str);
void init_exposchool(char * str);
exp_school_t expo_school_get_obj(int idx);
int expo_search(char * search);
void expo_school_deinit();

#endif