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
	gosrc "clubhouse/gosrc"
)

func main() {
	var h gosrc.Handler;

	s := &http.Server{
		Addr:           ":8080",
		Handler:        h,
		ReadTimeout:    10 * time.Second,
		WriteTimeout:   10 * time.Second,
		MaxHeaderBytes: 1 << 20,
	}
	s.ListenAndServe()
}

	// router, err := goczmq.NewRouter("tcp://*:1883")
	// if err != nil {
	// 	log.Fatal(err)
	// }
	// defer router.Destroy()

	// http.HandleFunc("/", func(w http.ResponseWriter, req * http.Request) {
	// 	index , err := os.Open("www/index.html")
	// 	if err != nil {
	// 		http.Error(w, "File not found", 404)
	// 		return
	// 	}
	// 	FileHeader := make([]byte, 512)
	// 	index.Read(FileHeader)

		//FileContentType := http.DetectContentType(FileHeader)

		// FileStat, _ := index.Stat()                     //Get info from file
		// FileSize := strconv.FormatInt(FileStat.Size(), 10) //Get file size as a string

		//Send the headers
		// w.Header().Set("Content-Disposition", "attachment; filename=index.html")
		// w.Header().Set("Content-Type", FileContentType)
		// w.Header().Set("Content-Length", FileSize)

		//Send the file
		//We read 512 bytes from the file already, so we reset the offset back to 0
	// 	index.Seek(0, 0)
	// 	io.Copy(w, index) //'Copy' the file to the client
	// 	return
	// })

	// fmt.Println(gosrc.TEMPLATE_PARENT)
	// http.HandleFunc("/hello", func(w http.ResponseWriter, req *http.Request) {
	// 	fmt.Fprintf(w, "hello\n")
	// })

	// http.HandleFunc("/headers", func(w http.ResponseWriter, req *http.Request) {
	// 	fmt.Fprintf(w, "header\n")
	// })

	// http.ListenAndServe(":80", nil)