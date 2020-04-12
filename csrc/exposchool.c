#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cjson/cJSON.h>
#include <string.h>
#include <assert.h>
#include "exposchool.h"
#include "json.h"

static exp_school_t * REF_LIST       = NULL;
static int            REF_LIST_LEN   = 0;
static exp_index_t  * SORT_LISTS     = NULL;
static int            SORT_LISTS_LEN = 0;
static exp_index_t    POP_LIST; 

void exposchool_get_href(cJSON * json, exp_school_t * school);
void exposchool_get_idx(cJSON * json, exp_index_t * all_index, int i);
int expo_word_len(char * str);
char * expo_str_idx(char * str, int idx);
void sort_temp(exp_school_t * temp_ref, int temp_ref_len);
void free_exp_idx(exp_index_t ** a_list, int len);

void free_exp_lint(exp_lint_t * list) {
	for (exp_lint_t * curr = list; curr != NULL; list = curr) {
		curr = curr->next;
		free(list);
	}
}

void sort_temp(exp_school_t * temp_ref, int temp_ref_len) {
	int max_word = 0;
	for (int i = 0; i < temp_ref_len; i++) {
		int word_len = expo_word_len(temp_ref[i].name);
		max_word = word_len > max_word ? word_len: max_word; 
	}

	size_t alloc_size;
	exp_index_t * sort_idx = malloc(alloc_size = (max_word * sizeof(*sort_idx)));
	if (sort_idx != NULL) {
		memset(sort_idx, 0, alloc_size);
	}

	for (int i = 0; i < temp_ref_len && sort_idx != NULL; i++) {
		int word_len = expo_word_len(temp_ref[i].name);
		for (int j = 0; j < word_len; j++) {
			exp_lint_t ** a_node = sort_idx[j].list == NULL ? &sort_idx[j].list: sort_idx[j].list_end;
			exp_lint_t * node = malloc(sizeof(*node));
			if (node != NULL) {
				memset(node, 0, sizeof(*node));
				node->value = temp_ref[i].id;
				sort_idx[j].list_end = &node->next;
				sort_idx[j].len++;
				(*a_node) = node;
			}
			if (word_len == 12) {
				printf("from 0 to %d, %s start: %s\n", word_len, temp_ref[i].name, expo_str_idx(temp_ref[i].name, j));
			}
		}
	}
	
	for (int i = 0; i < max_word; i++) {
		int * idx = malloc(alloc_size = (sort_idx[i].len * sizeof(*idx)));
		if (idx != NULL) {
			memset(idx, 0, alloc_size);
			for (int j = 0; j < sort_idx[i].len; j++) {
				exp_lint_t * node = sort_idx[i].list;
				sort_idx[i].list = node->next;
				idx[j] = node->value;
				free(node);
			}
		}
		sort_idx[i].idx = idx;
	}


	// sort method here
	free(sort_idx);

	printf("maxword: %d\n", max_word);
}

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
	cJSON * curr = NULL, * ref = NULL, * sort = NULL, * pop = NULL, * j = NULL;
	int temp_ref_len = 0, sort_len = 0;
	exp_school_t * temp_ref = NULL;
	exp_index_t * sort_idx = NULL;
	exp_index_t pop_idx;
	size_t alloc_size;
	int i = 0;

	ref = cJSON_DetachItemFromObjectCaseSensitive(json, "ref");
	if (cJSON_IsArray(ref)) {
		temp_ref_len = cJSON_GetArraySize(ref);
		temp_ref = malloc(alloc_size = (temp_ref_len * sizeof(*temp_ref)));
	}

	if (temp_ref != NULL) {
		memset(temp_ref, 0, alloc_size);
		i = 0;
		cJSON_ArrayForEach(curr, ref) {
			if (cJSON_IsObject(curr)) {
				j = cJSON_GetObjectItem(curr, "id");
				if (cJSON_IsNumber(j)) {
					temp_ref[i].id = j->valueint;
				}
				j = cJSON_GetObjectItem(curr, "name");
				if (cJSON_IsString(j)) {
					temp_ref[i].name = j->valuestring;
					j->valuestring = NULL;
				}
				j = cJSON_GetObjectItem(curr, "online");
				if (cJSON_IsBool(j)) {
					temp_ref[i].online = cJSON_IsTrue(j);
				}
				j = cJSON_GetObjectItem(curr, "pop");
				if (cJSON_IsNumber(j)) {
					temp_ref[i].pop = j->valueint;
				}
				j = cJSON_GetObjectItem(curr, "href");
				exposchool_get_href(j, &temp_ref[i]);
				i++;
			}
		}
	}
	REF_LIST     = temp_ref;
	REF_LIST_LEN = temp_ref_len;
	cJSON_Delete(ref);

	sort_temp(temp_ref, temp_ref_len);

	sort = cJSON_DetachItemFromObjectCaseSensitive(json, "sort");
	// if (cJSON_IsArray(sort)) {
	// 	sort_len = cJSON_GetArraySize(sort);
	// 	printf("len: %d\n", sort_len);
	// 	sort_idx = malloc(alloc_size = (sort_len * sizeof(*sort_idx)));
	// }

	// if (sort_idx != NULL) {
	// 	memset(sort_idx, 0 , alloc_size);
	// 	i = 0;
	// 	cJSON_ArrayForEach(curr, sort) {
	// 		exposchool_get_idx(curr, sort_idx, i);
	// 		i++;
	// 	}
	// }
	// SORT_LISTS     = sort_idx;
	// SORT_LISTS_LEN = sort_len;
	cJSON_Delete(sort);

	pop = cJSON_DetachItemFromObjectCaseSensitive(json, "pop");
	if (cJSON_IsArray(pop)) {
		exposchool_get_idx(pop, &pop_idx, 0);
	}
	cJSON_Delete(pop);
	POP_LIST = pop_idx;

	cJSON_Delete(json);
}

void exposchool_get_idx(cJSON * json, exp_index_t * all_index, int i) {
	size_t alloc_size = 0;
	cJSON * curr = NULL;
	int j = 0;

	if (all_index != NULL && cJSON_IsArray(json)) {
		all_index[i].len = cJSON_GetArraySize(json);
		all_index[i].idx = malloc(alloc_size = (all_index[i].len * sizeof(*all_index[i].idx)));
	}

	if (all_index[i].idx != NULL) {
		memset(all_index[i].idx, 0 , alloc_size);
		cJSON_ArrayForEach(curr, json) {
			if (cJSON_IsNumber(curr))  {
				all_index[i].idx[j] = curr->valueint;
			}
			j++;
		}
	}
}

void exposchool_get_href(cJSON * json, exp_school_t * school) {
	school->href_len  = cJSON_GetArraySize(json);
	size_t alloc_size = school->href_len * sizeof(exp_lstring_t);
	int i = 0;
	exp_lstring_t * href_temp = NULL;
	cJSON * curr = NULL, *l = NULL;

	if (cJSON_IsArray(json) && school != NULL) {
		href_temp = malloc(alloc_size);
	}

	if (href_temp != NULL) {
		memset(href_temp, 0, alloc_size);
		cJSON_ArrayForEach(curr, json) {
			l = cJSON_GetObjectItem(curr, "msg");
			if (cJSON_IsString(l)) {
				href_temp[i].msg = l->valuestring;
				l->valuestring = NULL;
			}
			l = cJSON_GetObjectItem(curr, "href");
			if (cJSON_IsString(l)) {
				href_temp[i].href = l->valuestring;
				l->valuestring = NULL;
			}
			i++;
		}
	}

	school->href = href_temp;
}

void expo_school_deinit() {
	free_exp_school(&REF_LIST, REF_LIST_LEN);
	free_exp_idx(&SORT_LISTS, SORT_LISTS_LEN);
	if (POP_LIST.idx != NULL) {
		free(POP_LIST.idx);
	}
}

exp_school_t expo_school_get_obj(int idx) {
	if (idx >= 0 && idx < REF_LIST_LEN) {
		bool found = false;
		if (REF_LIST[idx].id == idx) {
			return REF_LIST[idx];
		}
		else {
			// local search
			if (idx > 0 && REF_LIST[idx - 1].id == idx) {
				return REF_LIST[idx - 1];
			}
			else if ((idx - 1) < REF_LIST_LEN && REF_LIST[idx + 1].id == idx) {
				return REF_LIST[idx - 1];
			}
			else {
				for (int i = 0; i < REF_LIST_LEN; i++) {
					if (REF_LIST[i].id == idx) {
						return REF_LIST[i];
					}
				}
			}
		}
	}
	return (exp_school_t) {};
}

void expo_append_lint(exp_lint_t ** a_list, int val) {
	if (a_list != NULL) {
		for (; (*a_list) != NULL && (*a_list)->value != val; a_list = &(*a_list)->next) {}
		if ((*a_list) == NULL) {
			size_t alloc_size;
			(*a_list) = malloc(alloc_size = sizeof(**a_list));
			if ((*a_list) != NULL) {
				memset(*a_list, 0 , alloc_size);
				(*a_list)->value = val;
			}
		}
	}
}

int expo_strcmp(char * str1, char * str2) {
	if (str1 != NULL && str2 != NULL) {
		int i = 0;
		for (; str1[i] != ' ' && str1[i] != '\0' &&
			   str2[i] != ' ' && str2[i] != '\0'; i++) {
			if (str1[i] != str2[i])	 {
				return str2[i] - str1[i];
			}
		}
		// if (str2[i] != '\0' || str2[i] != ' ') {
		// 	return str2[i] - str1[i];
		// }
		return 0;
	}
	else {
		printf("error\n");
	}
	return -1;
}

char * expo_str_idx(char * str, int idx) {
	for (int i = 0; str != NULL && i < idx; i++) {
		str = strchr(str, ' ');
		str = str != NULL ? str + 1: NULL;
	}
	return str;
}

int expo_word_len(char * str) {
	int i = 0;
	for (; str != NULL; i++) {
		str = strchr(str, ' ');
		str = str != NULL ? str + 1: NULL;
	}
	return i;
}

void expo_binary_search(int start, int end, char * search, int idx) {
	// printf("index: %d\n", idx);
	// printf("start: %d, end: %d\n", start, end);
	if (start >= end) {
		return;
	}

	int middle = (start + end) / 2;
	int id = SORT_LISTS[idx].idx[middle];
	char * str1 = expo_school_get_obj(id).name;

	char * cpstr1 = str1;

	// printf("start: %s\n", str1);
	str1 = expo_str_idx(str1, idx);

	int direction = expo_strcmp(str1, search);
	if (direction < 0) {
		printf("left str1: %s, search: %s\n", str1, search);
		printf("left str1 + 1: %s, search: %s\n", 
			expo_str_idx(expo_school_get_obj(SORT_LISTS[idx].idx[middle + 1]).name, idx), 
			search);
		expo_binary_search(start, middle - 1, search, idx);
	}
	else if (direction > 0) {
		printf("right str1: %s, search: %s\n", str1, search);
		expo_binary_search(middle + 1, end, search, idx);
	}
	else {
		printf("append to list: %s\n", expo_school_get_obj(id).name);
		// append
	}
}

int expo_search(char * search) {
	for (;search != NULL; search = strchr(search, ' ')) {
		printf("sort len: %d\n", SORT_LISTS_LEN);
		for (int i = 0; i < SORT_LISTS_LEN - 1; i++) {
			printf("\nNEXT %d, %d\n", i, SORT_LISTS[i].len);
			expo_binary_search(0, SORT_LISTS[i].len, search, i + 1);
		}
	}
}
