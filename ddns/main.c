#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <getdns/getdns_extra.h>

#include <getdns/getdns_ext_libevent.h>
#include <getdns/getdns_ext_libev.h>
#include <getdns/getdns_ext_libuv.h>
#include <getdns/getdns.h>

int main(int argc, char* argv[]) {
	getdns_return_t r;

	getdns_context * ctxt = NULL;
	r = getdns_context_create(&ctxt, 1);
	assert(r == 0);

	getdns_context_destroy(ctxt);

	return EXIT_SUCCESS;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
