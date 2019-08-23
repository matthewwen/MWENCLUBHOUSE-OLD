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

int main(int argc, char *argv[]) {
	int port = 5000;


	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	listen(listenfd, 10);

	char sendBuff[1025] = "Hello There, You are connected to server";
	socklen_t len = sizeof(serv_addr);
	for (int connfd = -1; true; connfd = -1) {
		if ((connfd = accept(listenfd, (struct sockaddr*) &serv_addr, &len) >= 0)) {
			// getting message from the server
			write(connfd, sendBuff, strlen(sendBuff));
			//printf("the buff msg: %s\n", sendBuff);
			close(connfd);
			// int fdimg = open("ubuntu-18.04.3-desktop-amd64.iso", O_RDONLY);
			// sendfile(connfd, fdimg, NULL, 4000);
			// close(fdimg);
		}
	}
}
