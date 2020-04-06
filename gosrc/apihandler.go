package clubhouse

import (
	/* 
	#cgo CFLAGS: -O6 -std=c99 -Wshadow -Wvla -pedantic -Wall
	#cgo LDFLAGS: -lsqlite3 
	#include <stdio.h>
	#include <stdlib.h>

	void getproj_handler(char * pkg_name, char * grid_name) {
		printf("pkg_name: %s, grid_name: %s\n", pkg_name, grid_name);
	}
	*/
	"C"
	"net/http"
	str "strings"
	// "fmt"
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
			C.getproj_handler(C.CString("hi"), C.CString("hi lol"))
			//getproj_handler(wsi, found, r);
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