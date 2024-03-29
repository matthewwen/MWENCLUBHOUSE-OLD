package main

import (
	"fmt"
	"net/http"
	"time"
	_ "strings"
	"os"
	_ "io"
	str "strings"
	//"strconv"

	goczmq "gopkg.in/zeromq/goczmq.v4"
	"log"
	gosrc "github.com/matthewwen/MWENCLUBHOUSE/gosrc"
)

func redirect(w http.ResponseWriter, req * http.Request) {
	http.Redirect(w, req, "https://" + req.Host + req.URL.String(), http.StatusMovedPermanently)
}

func main() {
	var h gosrc.Handler;

	router, err := goczmq.NewRouter("tcp://*:1883")
	if err != nil {
		log.Fatal(err)
	}
	defer router.Destroy()

	argsWithProg := os.Args
	var dev bool = true 
	if (len(argsWithProg) == 2) {
		dev = str.Compare(argsWithProg[1], "dep") != 0
	}

	go gosrc.Init_Clubhouse()
	defer gosrc.Close_Clubhouse()

	var s *http.Server;
	if (dev) {
		fmt.Println("Development")
		s = &http.Server{
			Addr:           ":80",
			Handler:        h,
			ReadTimeout:    10 * time.Second,
			WriteTimeout:   10 * time.Second,
			MaxHeaderBytes: 1 << 20,
		}
		s.ListenAndServe()
	} else {
		fmt.Println("Deploy")
		s = &http.Server{
			Addr:           ":443",
			Handler:        h,
			ReadTimeout:    10 * time.Second,
			WriteTimeout:   10 * time.Second,
			MaxHeaderBytes: 1 << 20,
		}
		go http.ListenAndServe(":80", http.HandlerFunc(redirect))
		s.ListenAndServeTLS("/etc/letsencrypt/live/api.matthewwen.com/fullchain.pem", "/etc/letsencrypt/live/api.matthewwen.com/privkey.pem")
	}
}
