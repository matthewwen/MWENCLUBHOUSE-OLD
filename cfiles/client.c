#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		printf("\n Error : Could not create socket \n");
		return 1;
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5000);

	if(inet_pton(AF_INET, "10.15.231.21", &serv_addr.sin_addr)<=0) {
		printf("\n inet_pton error occured\n");
		return 1;
	}

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("Error : Connect Failed\n");
		return 1;
	}

	char recvBuff[1024];
	memset(recvBuff, 0,sizeof(recvBuff));
	int n = 0;
	while ((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0) {
		printf("in the loop : %d\n", n);
		recvBuff[n] = 0;
		if(fputs(recvBuff, stdout) == EOF) {
			printf("\n Error : Fputs error\n");
		}
	}

	if(n < 0) {
		printf("Read error\n");
	}

	return EXIT_SUCCESS;
}