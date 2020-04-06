package clubhouse

import (
	"fmt"
	"net/http"
	"strings"
)

type Handler func(http.ResponseWriter, * http.Request)

func (h Handler) ServeHTTP(w http.ResponseWriter, r * http.Request) {
	// fmt.Println(w)
	// fmt.Println(r)
	if strings.Compare(r.Method, "GET") == 0 {
		fmt.Println("Get")
	} else if strings.Compare(r.Method, "PUT") == 0 {
		fmt.Println("put")
	} else if strings.Compare(r.Method, "POST") == 0 {
		fmt.Println("post")
	}
	fmt.Println(r.Method, r.URL)
}