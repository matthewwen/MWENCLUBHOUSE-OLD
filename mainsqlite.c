#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "msqlite.h"
#include "webdatabase.h"
#include "json.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		return EXIT_FAILURE;
	}
	if (strcmp(argv[1], "-h") == 0) {
		if (argc > 2) {
			if 	(strcmp(argv[2], "update") == 0) {
				if (argc == 5) {
					char * href  = argv[3];
					bool visible = atoi(argv[4]) == 0 ? false: true;
					set_is_visible(href, visible);
				}
			}
		}

		jobject * data = get_web_record_href();
		for (jlist * curr = data->data.list; curr != NULL; curr = curr->next) {
			jobject * root = curr->data.obj;

			// get id
			jobject * temp = get_jobject("id", root);
			int id = temp != NULL  && temp->type == NUM ? temp->data.num: -1;

			// get href
			temp = get_jobject("href", root);
			char * href =  temp != NULL && temp->type == TEXT ? temp->data.txt: "Error";

			// get visibility
			temp = get_jobject("visible", root);
			bool is_visible = temp != NULL && temp->type == CON ? temp->data.cond: false;

			printf("id: %d, href: %s, visible: %s\n", id, href, is_visible ? "true": "false");
		}
		free_json(&data);
	}
	else if (strcmp(argv[1], "-i") == 0) {
		printf("it is for the images\n");
	}
	else if (strcmp(argv[1], "-g") == 0) {
		printf("it is for the grid\n");
	}

	return EXIT_SUCCESS;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
