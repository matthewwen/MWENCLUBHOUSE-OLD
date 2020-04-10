#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cjson/cJSON.h>
#include <string.h>
#include <assert.h>
#include "exposchool.h"
#include "json.h"

static exp_school_t * REF_LIST     = NULL;
static int            MAX_WORD     = 0;
static exp_index_t  * SORTED_LISTS = NULL;
static exp_index_t    POP_LIST; 

void free_exp_school(exp_school_t ** a_list, int len) {
	if ((a_list != NULL) && ((*a_list) != NULL)) {
		exp_school_t * list = *a_list;
		for (int i = 0; i < len; i++) {
			if (list[i].name != NULL) {
				free(list[i].name);
			}
			if (list[i].href != NULL) {
				for (int j = 0; j < list[i].href_len; j++)	 {
					if (list[i].href[j].msg != NULL) {
						free(list[i].href[j].msg);
					}
					if (list[i].href[j].href != NULL) {
						free(list[i].href[j].href);
					}
				}
				free(list[i].href);
			}
		}
		free(list);
		*a_list = NULL;
	}
}

void free_exp_idx(exp_index_t ** a_list, int len) {
	exp_index_t * list = *a_list;
	for (int i = 0; i < len; i++) {
		if (list[i].idx != NULL) {
			free(list[i].idx);
		}
	}
	free(list);
	*a_list = NULL;
}

void init_exposchool(char * str) {
	cJSON * json = cJSON_Parse(str);
	cJSON * i = NULL;
	size_t alloc_size;
	if (json != NULL && cJSON_IsObject(json)) {
		cJSON * ref = cJSON_DetachItemFromObjectCaseSensitive(json, "ref");
		if (ref != NULL && cJSON_IsArray(ref)) {
			cJSON * j = NULL;
			int len = cJSON_GetArraySize(ref);
			alloc_size = len * sizeof(exp_school_t);
			exp_school_t * temp_ref = malloc(alloc_size);

			if (temp_ref != NULL) {
				memset(temp_ref, 0, alloc_size);
				int idx = 0;

				cJSON_ArrayForEach(i, ref) {
					if (i != NULL && cJSON_IsObject(i)) {
						j = cJSON_GetObjectItem(i, "id");
						if (j != NULL && cJSON_IsNumber(j)) {
							temp_ref[idx].id = j->valueint;
						}
						j = cJSON_GetObjectItem(i, "name");
						if (j != NULL && cJSON_IsString(j)) {
							temp_ref[idx].name = j->valuestring;
							j->valuestring = NULL;
						}
						j = cJSON_GetObjectItem(i, "online");
						if (j != NULL && cJSON_IsBool(j)) {
							temp_ref[idx].online = cJSON_IsTrue(j);
						}
						j = cJSON_GetObjectItem(i, "pop");
						if (j != NULL && cJSON_IsNumber(j)) {
							temp_ref[idx].pop = j->valueint;
						}
						j = cJSON_GetObjectItem(i, "href");
						if (j != NULL && cJSON_IsArray(j)) {
							int href_len = cJSON_GetArraySize(j);
							alloc_size = href_len * sizeof(exp_lstring_t);
							exp_lstring_t * href_temp = malloc(alloc_size);
							temp_ref[idx].href_len = href_len;

							if (href_temp != NULL) {
								memset(href_temp, 0, alloc_size);
								int j_idx = 0;
								cJSON * k = NULL;
								cJSON_ArrayForEach(k, j) {
									cJSON * l = NULL;
									if (cJSON_IsObject(k)) {
										l = cJSON_GetObjectItem(k, "msg");
										if (l != NULL && cJSON_IsString(l)) {
											assert(j_idx < href_len);
											href_temp[j_idx].msg = l->valuestring;
											l->valuestring = NULL;
										}
										l = cJSON_GetObjectItem(k, "href");
										if (l != NULL && cJSON_IsString(l)) {
											href_temp[j_idx].href = l->valuestring;
											l->valuestring = NULL;
										}
										j_idx++;
									}
								}
							}
							temp_ref[idx].href = href_temp;
						}
						idx++;
					}
				}
				free_exp_school(&temp_ref, len);
			}
		}
		cJSON_Delete(ref);

		cJSON * sort = cJSON_DetachItemFromObjectCaseSensitive(json, "sort");
		assert(sort != NULL);
		if (sort != NULL && cJSON_IsArray(sort)) {
			int all_len = cJSON_GetArraySize(sort);
			exp_index_t * all_index = malloc(all_len * sizeof(*all_index));
			
			if (all_index != NULL) {
				memset(all_index, 0, all_len * sizeof(*all_index));
				int a = 0;
				cJSON_ArrayForEach(i, sort) {
					if (i != NULL && cJSON_IsArray(i)) {
						int len = cJSON_GetArraySize(i);
						int * idx = malloc(alloc_size = (len * sizeof(*idx)));
						if (idx != NULL) {
							memset(idx, 0, alloc_size);
							cJSON * j = NULL;
							int b = 0;
							cJSON_ArrayForEach(j, i) {
								if (j != NULL && cJSON_IsNumber(j)) {
									idx[b] = j->valueint;
								}
								b++;
							}
							all_index[a].len = len;
							all_index[a].idx = idx;
							a++;
						}
					}
				}
			}
			free_exp_idx(&all_index, all_len);
		}
		cJSON_Delete(sort);

		cJSON * pop = cJSON_DetachItemFromObjectCaseSensitive(json, "pop");
		if (pop != NULL && cJSON_IsArray(pop)) {
			int len_pop = cJSON_GetArraySize(pop);
			int * pop_idx = malloc(alloc_size = (len_pop * sizeof(*pop_idx)));
			if (pop_idx != NULL) {
				int a = 0;
				cJSON_ArrayForEach(i, pop) {
					if (i != NULL && cJSON_IsNumber(i)) {
						pop_idx[a] = i->valueint;
					}
					a++;
				}
			}
			free(pop_idx);
		}
		cJSON_Delete(pop);
	}
	cJSON_Delete(json);
}