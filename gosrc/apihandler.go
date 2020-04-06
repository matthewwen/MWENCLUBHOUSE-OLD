package clubhouse

import (
	/* 
	#cgo CFLAGS: -O6 -std=c99 -Wshadow -Wvla -pedantic -Wall -I .././cheader
	#cgo LDFLAGS: -lsqlite3 -L .././object -lclubhouse
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "json.h"
	#include "webdatabase.h"

	void format_href(char * href) {
		for (int i = 0; href[i] != '\0'; i++) {
			if (href[i] == '-') {
				href[i] = '_';
			}
		}
	}

	char * getproj_handler(char * pkg_name, char * grid_name) {
		jobject * obj = NULL;
		char * str    = NULL;
		
		int count = strlen(pkg_name) + strlen(grid_name) + 1;
		char * href = malloc(count * sizeof(*href));
		if (href != NULL) {
			sprintf(href, "%s%s", pkg_name, grid_name);
			format_href(href);
			obj = get_project_view(href);
			free(href);
		}

		append_jobject(&obj, "gridName", TEXT, (data_t) {.txt = grid_name});
		json_tostring(&str, obj, NULL);
		free_json(&obj);

		return str;
	}

	void free_str(void * p) {
		if (p != NULL) {
			free(p);
		}
	}
	*/
	"C"
	"net/http"
	str "strings"
	"unsafe"
	"fmt"
)

var (
	TEMPLATE_PARENT string = "www/other/"
	TEMPLATE_INDEX  string = "/index.html"
	TEMPLATE_MAINJS string = "/main.js"
	TEMPLATE_CSS    string = "/main.css"
)

func (h * Handler) handle_gapi_request(found * bool, w http.ResponseWriter, r * http.Request) {
	var url string = r.URL.Path

	if (*found == false) {
		if (str.Compare("/webedit", url) == 0 || str.Compare("/webedit/", url) == 0) {
		} else if (str.Compare("/mwenCreatePROJ", url) == 0 || str.Compare("/mwenCreatePROJ/", url) == 0) {
			//createproj_handler(wsi, found, r);
		} else if (str.Compare("/mwenGetPROJ", url) == 0 || str.Compare("/mwenGetPROJ/", url) == 0) {
			query_pkg  := r.URL.Query()["pkg-name"]
			query_grid := r.URL.Query()["grid-name"]
			if len(query_pkg) > 0  && len(query_grid) > 0 {
				a := C.getproj_handler(C.CString(query_pkg[0]), C.CString(query_grid[0]))
				if unsafe.Pointer(a) != nil {
					defer C.free(unsafe.Pointer(a))
					*found = true
					w.Header().Set("Content-Type", "application/json")
					fmt.Fprintf(w, C.GoString(a));
				}
			}
		}
		// else if (strncmp(MOREYIOT, url, strlen(MOREYIOT)) == 0) {
		// 	morey_handler(url, wsi, found, r);
		// }
		// else if (strncmp(TODOURL, url, strlen(TODOURL)) == 0) {
		// 	todo_handler(url, wsi, found, r);
		// }	
	}

}

func (h Handler) send_file(path string, found * bool, w http.ResponseWriter, r * http.Request) {

}