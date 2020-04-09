#ifndef _M_PYTHON_H
#define _M_PYTHON_H

#include <stdio.h>
#include <stdlib.h>

void init_mpython();

// pageview handler
char * get_pageview();

// expensive online school
char * get_college();
char * get_detail(long id);
char * search_query(char * query);

void close_mpython();

#endif