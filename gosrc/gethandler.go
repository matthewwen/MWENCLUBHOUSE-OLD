package clubhouse

import (
	_ "fmt"
	"net/http"
	// "strings"
)

func (h * Handler) handle_get_request(w http.ResponseWriter, r * http.Request) {
	var found bool = false
	h.handle_gweb_request(&found, w, r)
	h.handle_gapi_request(&found, w, r)

	if (!found) {
		http.Error(w, "Request Not Found", 404)
	}
}