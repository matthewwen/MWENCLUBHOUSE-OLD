#include "header.h"

int main(int argc, char* argv[]) {
	int port_to_use = 8245;
	int time_out = 0;
	struct passwd * nobody;

	struct sigaction sig = {.sa_flags = 0,
	                        .sa_handler = SIG_IGN};
	sigemptyset(&sig.sa_mask);
	sigaction(SIGHUP,&sig,NULL);
	sigaction(SIGPIPE,&sig,NULL);
	sigaction(SIGUSR1,&sig,NULL);
	sigaction(SIGUSR2,&sig,NULL);
	sigaction(SIGALRM,&sig,NULL);

	// check if conf file exist

	return EXIT_SUCCESS;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
