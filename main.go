package main

import (
	"fmt"
	"net/http"
)

func main() {

	fmt.Println("Main")

	http.HandleFunc("/hello", func(w http.ResponseWriter, req *http.Request) {
		fmt.Fprintf(w, "hello\n")
	})

	http.HandleFunc("/headers", func(w http.ResponseWriter, req *http.Request) {
		fmt.Fprintf(w, "header\n")
	})

	http.ListenAndServe(":80", nil)
}
