package clubhouse

import (
	"fmt"
	"net/http"
	str "strings"
	"os"
	"io"
	"strconv"
)

type Handler func(http.ResponseWriter, * http.Request)

func (h Handler) ServeHTTP(w http.ResponseWriter, r * http.Request) {
	// fmt.Println(w)
	// fmt.Println(r)
	var a_h * Handler = &h
	if str.Compare(r.Method, "GET") == 0 {
		a_h.handle_get_request(w, r)
	} else if str.Compare(r.Method, "PUT") == 0 {
		fmt.Println("put")
	} else if str.Compare(r.Method, "POST") == 0 {
		fmt.Println("post")
	}
}

func strrchr(path string, c byte) string {
	for i := len(path) - 1; i >= 0; i = i - 1 {
		if (path[i] == c) {
			return path[i:]
		}
	}
	return "" 
}

func strchr(path string, c byte) string {
	for i:= 0; i < len(path); i++ {
		if (path[i] == c) {
			return path[i:]
		}
	}
	return ""
}

func get_mime_type(path string) (string, bool) {
	end  := strrchr(path, '.');
	fmt.Println("end", end)
	if (str.Compare(end, ".js") == 0) {
		return "application/javascript" , true
	} else if (str.Compare(end, ".mjs") == 0) {
		return "text/javascript", true
	} else if (str.Compare(end, ".css") == 0 || str.Compare(end, ".scss") == 0) {
		return "text/css", true
	} else if (str.Compare(end, ".html") == 0) {
		return "text/html", true
	} else if (str.Compare(end, ".map" ) == 0) {
		return "application/octet-stream", true
	} else if (str.Compare(end, ".jpg") == 0) {
		return "image/jpg", true
	} else if (str.Compare(end, ".png") == 0) {
		return "image/png", true
	} else if (str.Compare(end, ".pdf") == 0) {
		return "application/pdf", true
	} else if (str.Compare(end, ".webmanifest") == 0) {
		return "application/manifest+json", true
	} else if (str.Compare(end, ".txt") == 0) {
		return "plain/text", true
	}
	return "plain/text", false
}

func send_file(path string, found * bool, w http.ResponseWriter, r * http.Request) int {
	*found = true
	file , err := os.Open(path)
	if err != nil {
		http.Error(w, "File not found", 404)
		return 1
	}
	FileHeader := make([]byte, 512)
	file.Read(FileHeader)

	FileContentType , _ := get_mime_type(path)
	fmt.Println(FileContentType)

	FileStat, _ := file.Stat()                     
	FileSize := strconv.FormatInt(FileStat.Size(), 10) 

	// Send the headers
	// w.Header().Set("Content-Disposition", "attachment; filename=" + path)
	w.Header().Set("Content-Type", FileContentType)
	w.Header().Set("Content-Length", FileSize)

	file.Seek(0, 0)
	io.Copy(w, file) 
	return 0
}