package clubhouse

import (
	"fmt"
	"net/http"
	str "strings"
/*
#include <stdio.h>
#include <stdlib.h>
#include "webdatabase.h"
*/
	"C"
)

var (
	HTML404 string = `<!DOCTYPE html>
<html>
	<head>
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<link rel="apple-touch-icon" sizes="180x180" href="/www/favicon/apple-touch-icon.png">
		<link rel="icon" type="image/png" sizes="32x32" href="/www/favicon/favicon-32x32.png">
		<link rel="icon" type="image/png" sizes="16x16" href="/www/favicon/favicon-16x16.png"> 
		<link rel="manifest" href="/www/favicon/site.webmanifest">

		<title>Create Project</title>
		<link rel="stylesheet" href="/www/css/mwen.css" />
		<link href="/www/css/material/material-components-web.min.css" rel="stylesheet">
		<script src="/www/js/material/material-components-web.min.js"></script>
		<link href="/www/css/material/font.css" rel="stylesheet">
		<link rel="stylesheet" href="https://fonts.googleapis.com/icon?family=Material+Icons">
	</head>

	<body>
		<div id="div-body" class="container"></div>
		<h1 style="text-align:center; margin: 20px 0;">404 Error. Error Getting Page</h1>
		<footer style="text-align: center;">Copyright &copy; 2020 Matthew Wen</footer>
	</body>
</html>`
)

func (h * Handler) handle_gweb_request(found *bool, w http.ResponseWriter, r * http.Request) int {
	n := 0
	if (*found) == false {
		url := r.URL.Path
		var www string = "/www/"
		// if str.Compare(www, r.URL.Path[0:len()])
		if (str.Compare("/", url) == 0) {
			n = send_file("www/index.html", found, w, r)
		} else if (str.Compare(www, url[:len(www)]) == 0) {
			n = send_file(url[1:], found, w, r)
		} else if (str.Compare("/createpkg", url) == 0 || str.Compare("/createpkg/", url) == 0) {
		} else if (str.Compare("/pageview", url) == 0 || str.Compare("/pageview/", url) == 0) {
			n = send_file("www/html/pageview.html", found, w, r)
		} else if len(url) > len("/pageview/") && (str.Compare("/pageview/", url[:len("/pageview/")]) == 0) {
			// getting ref
			ref := url[len("/pageview/"):len("/pageview/") + 10]

			isVisible := C.get_is_visible(C.CString(ref))
			fmt.Println("isVisible", isVisible)

			if (isVisible) {
				path := "../other/" + ref
				_, mime := get_mime_type(path)
				if mime == false {
					path += "/index.html"
				}
				n = send_file(path, found, w, r)
			} else {
				*found = true;
				http.Error(w, "Pageview Not Available", 404)
			}
		} else if (str.Compare("/resume", url) == 0 || str.Compare("/resume/", url) == 0) {
			n = send_file("www/pdf/matthewwen.pdf", found, w, r)
		}
	}
	return n
}