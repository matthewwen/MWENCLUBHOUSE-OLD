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
	serv_addr.sin_port = htons(5000);

	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	listen(listenfd, 10);

	char sendBuff[1025];
	memset(sendBuff, 0, sizeof(sendBuff));
	for (int connfd = -1; true; connfd = -1) {
		if ((connfd = accept(listenfd, (struct sockaddr*) NULL, NULL) >= 0)) {
			// getting message from the server
			//int bc = recv(listenfd, sendBuff, sizeof(sendBuff) - 1, 0);
			//sendBuff[bc] = 0;
			//printf("buff msg: %s\n", sendBuff);
			int fdimg = open("ubuntu-18.04.3-desktop-amd64.iso", O_RDONLY);
			sendfile(connfd, fdimg, NULL, 4000);
			close(connfd);
			close(fdimg);
		}
	}
}
