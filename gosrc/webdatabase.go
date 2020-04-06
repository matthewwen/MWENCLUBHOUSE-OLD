package clubhouse

import (
	// "fmt"
	_"net/http"
	// "strings"
)

func web_record_href(ref string, is_visible bool) {

}

// jobject
func get_web_record_href() {

}

func get_is_visible(href string) bool {
	return true
}

func set_is_visible(href string, is_visible bool) {

}

// finding grid table
type grid_type_t int
const (
	NONE    = 0
	NEWPAGE = 1
)

func insert_grid(table_name string, t grid_type_t) {

}

func set_grid_type(table_name string, t grid_type_t) {

}

func get_table_type(table_name string) grid_type_t {
	return NONE
}

// for project view
func set_project_view(table_name string, ref string, weight int, body string) {

}

// jobject
func get_project_view(table_name string) {

}

