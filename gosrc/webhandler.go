package clubhouse

import (
	_ "fmt"
	"net/http"
	str "strings"
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
			ref := url[len("/pageview/"):]
			path := "../other/" + ref
			_, mime := get_mime_type(path)
			if mime == false {
				path += "/index.html"
			}
			n = send_file(path, found, w, r)
		} else if (str.Compare("/resume", url) == 0 || str.Compare("/resume/", url) == 0) {
			n = send_file("www/pdf/matthewwen.pdf", found, w, r)
		}
	}
	return n
}