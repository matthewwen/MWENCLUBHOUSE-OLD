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
	#include "mpython.h"

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
	"strconv"
)

var (
	TEMPLATE_PARENT string = "www/other/"
	TEMPLATE_INDEX  string = "/index.html"
	TEMPLATE_MAINJS string = "/main.js"
	TEMPLATE_CSS    string = "/main.css"
)

func Init_Clubhouse() {
	C.init_mpython()
}

func Close_Clubhouse() {
	C.close_mpython()
}


func (h * Handler) handle_gapi_request(found * bool, w http.ResponseWriter, r * http.Request) {
	var url string = r.URL.Path

	if (*found == false) {
		if compare_url("/webedit", url) {
		} else if compare_url("/mwenCreatePROJ", url) {
			//createproj_handler(wsi, found, r);
		} else if compare_url("/mwenGetPROJ", url) {
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
		} else if compare_sub("/apipageview/", url) {
			url = url[len("/apipageview"):]
			if compare_url("/all", url) {
				a := C.get_pageview()
				if unsafe.Pointer(a) != nil {
					defer C.free(unsafe.Pointer(a))
					*found = true
					w.Header().Set("Content-Type", "application/json")
					fmt.Fprintf(w, C.GoString(a));
				} else {
					http.Error(w, "Server Error", 404)
				}
			}
		} else if compare_sub("/apiexposchool/", url) {
			w.Header().Set("Access-Control-Allow-Origin", "*")
			w.Header().Set("Access-Control-Allow-Headers", "*")
			url = url[len("/apiexposchool"):]
			if compare_url("/college", url) {
				a := C.get_college()
				send_c_buffer(w, a, found)
			} else if compare_url("/detail", url) {
				ids, ok := r.URL.Query()["id"]
				if ok && len(ids) > 0  {
					i64, err := strconv.ParseInt(ids[0], 10, 32)
					if err == nil {
						a := C.get_detail(C.long(i64))
						send_c_buffer(w, a, found)
					}
				} else {
					*found = true
					http.Error(w, "Error Searching", 404)
				}
			} else if compare_url("/addhref", url) {
			} else if compare_url("/search", url) {
				qs, ok := r.URL.Query()["query"]
				if ok && len(qs) > 0 {
					q := qs[0]
					a := C.search_query(C.CString(q))
					send_c_buffer(w, a, found)
				}
			} else if compare_url("/update", url) {
			}
		} else if compare_sub("/todo", url) {
			// fmt.Println(url)
			url = url[len("/todo"):]
			fmt.Println("todo url: ", url, len(url))
			if len(url) == 0 || str.Compare("/", url) == 0 {
				// send html page
				fmt.Println("send html file")
			} else if compare_url("/section", url) {
				// section
				fmt.Println("send section")
			} else if compare_url("/task", url) {
				// task
				fmt.Println("send task")
			} else if compare_url("/calendar", url) {
				// calendar
				fmt.Println("send calendar")
			}
		}
		
		// else if (strncmp(MOREYIOT, url, strlen(MOREYIOT)) == 0) {
		// 	morey_handler(url, wsi, found, r);
		// }
	}

}

func send_c_buffer(w http.ResponseWriter, a * C.char, found * bool) {
	*found = true
	if unsafe.Pointer(a) != nil {
		defer C.free(unsafe.Pointer(a))
		w.Header().Set("Content-Type", "application/json")
		fmt.Fprintf(w, C.GoString(a));
	} else {
		http.Error(w, "Server Error", 404)
	}
}