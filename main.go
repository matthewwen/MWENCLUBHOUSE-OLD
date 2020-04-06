package main

import (
	_ "fmt"
	"net/http"
	"time"
	_ "strings"
	_ "os"
	_ "io"
	//"strconv"

	// goczmq "gopkg.in/zeromq/goczmq.v4"
	gosrc "github.com/matthewwen/MWENCLUBHOUSE/gosrc"
)

func main() {
	var h gosrc.Handler;

	s := &http.Server{
		Addr:           ":80",
		Handler:        h,
		ReadTimeout:    10 * time.Second,
		WriteTimeout:   10 * time.Second,
		MaxHeaderBytes: 1 << 20,
	}
	s.ListenAndServe()
}