#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <sys/sendfile.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/epoll.h>

static void panelconn_signal_handler(int signo) {

}

int main(int argc, char *argv[]) {
	int port = 5000;

	char sendBuff[1025];
	memset(sendBuff, 0, sizeof(sendBuff));

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	listen(listenfd, 10);

	int connfd = 0;
	struct sockaddr cli_addr;
	socklen_t cli_size = sizeof(cli_addr);
	while(true) {
		connfd = accept(listenfd, (struct sockaddr*) &cli_addr, &cli_size);
		printf("client size: %d, data: %s\n", cli_size, cli_addr.sa_data);

        int fdimg = open("file.txt", O_RDONLY);
        sendfile(connfd, fdimg, NULL, 4000);
        close(fdimg);

		close(connfd);
	}
}