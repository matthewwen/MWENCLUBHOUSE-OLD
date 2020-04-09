#include "Python.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static PyObject * PAGE_MODULE = NULL;
static PyObject * EXPSCHOOL_MODULE = NULL;

void init_mpython() {
	setenv("PYTHONPATH", "./py", 1);
	Py_Initialize();
	PAGE_MODULE      = PyImport_ImportModule("pageview");
	EXPSCHOOL_MODULE = PyImport_ImportModule("exposchool");
	assert(PAGE_MODULE != NULL);
	assert(EXPSCHOOL_MODULE != NULL);

	PyObject * function = PyObject_GetAttrString(EXPSCHOOL_MODULE, "init_module");
	PyObject * myresult = PyObject_CallObject(function, NULL);
	Py_DECREF(function);
	Py_DECREF(myresult);
}

char * get_str_from_pyobject(PyObject * myresult) {
	// put result inside allocated buffer
	Py_UCS4 * p_buffer = NULL;
	ssize_t length = 0;
	if (myresult != NULL) {
		length = PyUnicode_GetLength(myresult);
		ssize_t alloc_size = (length + 1) * sizeof(*p_buffer);
		p_buffer = malloc(alloc_size);
		if (p_buffer != NULL) {
			memset(p_buffer, 0, alloc_size);
			Py_UCS4 * temp = PyUnicode_AsUCS4(myresult, p_buffer, alloc_size, 0);
			assert(temp != NULL);
			Py_DECREF(myresult);
		}
	}

	// put into C char *
	char * buffer = NULL;
	if (p_buffer != NULL) {
		size_t alloc_size = (length + 1) * sizeof(*buffer);
		buffer = malloc(alloc_size);
		if (buffer != NULL) {
			memset(buffer, 0, alloc_size);
			for (ssize_t i = 0; i < length; i++) {
				buffer[i] = p_buffer[i];
			}
		}
		free(p_buffer);
	}
	return buffer;
}

char * get_pageview() {
	// gets result from python program
	assert(PAGE_MODULE != NULL);
	PyObject * function = PyObject_GetAttrString(PAGE_MODULE, "pageview_settings");
	PyObject * myresult = PyObject_CallObject(function, NULL);
	Py_DECREF(function);

	// put result inside allocated buffer
	return get_str_from_pyobject(myresult);
}

char * get_college() {
	assert(EXPSCHOOL_MODULE != NULL);
	PyObject * function = PyObject_GetAttrString(EXPSCHOOL_MODULE, "get_college");
	PyObject * myresult = PyObject_CallObject(function, NULL);
	Py_DECREF(function);

	// put result inside allocated buffer
	return get_str_from_pyobject(myresult);
}

char * get_detail(long id) {
	assert(EXPSCHOOL_MODULE != NULL);
	PyObject * function = PyObject_GetAttrString(EXPSCHOOL_MODULE, "get_detail");
	PyObject * args     = PyTuple_Pack(1, PyLong_FromLong(id));
	PyObject * myresult = PyObject_CallObject(function, args);
	Py_DECREF(args);
	Py_DECREF(function);

	// put result inside allocated buffer
	return get_str_from_pyobject(myresult);
}

char * search_query(char * query) {
	assert(EXPSCHOOL_MODULE != NULL);
	PyObject * function = PyObject_GetAttrString(EXPSCHOOL_MODULE, "search_query");
	PyObject * args     = PyTuple_Pack(1, PyUnicode_FromString(query));
	PyObject * myresult = PyObject_CallObject(function, args);
	Py_DECREF(args);
	Py_DECREF(function);

	// put result inside allocated buffer
	return get_str_from_pyobject(myresult);
}

void close_mpython() {
	Py_DECREF(PAGE_MODULE);
	Py_DECREF(EXPSCHOOL_MODULE);
	Py_Finalize();
}

int main() {
	init_mpython();
	close_mpython();
	return 0;
}