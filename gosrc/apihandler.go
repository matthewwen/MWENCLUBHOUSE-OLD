package clubhouse

import (
	// "fmt"
	"net/http"
	// "strings"
)

var (
	TEMPLATE_PARENT string = "www/other/"
	TEMPLATE_INDEX  string = "/index.html"
	TEMPLATE_MAINJS string = "/main.js"
	TEMPLATE_CSS    string = "/main.css"
)

func (h * Handler) handle_gapi_request(found * bool, w http.ResponseWriter, r * http.Request) {

}

func (h Handler) send_file(path string, found * bool, w http.ResponseWriter, r * http.Request) {

}