#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/sendfile.h>

#define SERVER_PORT 8080

char info[] = 
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>Title</title>\r\n";

char test[] = "hello there\n" "matthew wen" "okay";


int main(int argc, char * argv[]) {
    char  buf[2048];

    int fd_server;
    if((fd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("error\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr = {.sin_family = AF_INET, 
                                      .sin_addr = {.s_addr = INADDR_ANY}, 
                                      .sin_port = htons(SERVER_PORT)};
    
    if (bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        printf("bind error\n");
        close(fd_server);
        return EXIT_FAILURE;
    }

    if (listen(fd_server, 10) == -1) {
        printf("listen error\n");
        close(fd_server);
        return EXIT_FAILURE;
    }

    int fd_client;
    struct sockaddr_in client_addr;
    socklen_t sin_len = sizeof(client_addr);
    while (true) {
        fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);

        if (fd_client == -1) {
            printf("connection failure\n");
            continue;
        }

        printf("Got client connection ... \n");

        if (!fork()) {
            // child process
            close(fd_server);
            memset(buf, 0, 2048);
            read(fd_client, buf, 2047);

            printf("%s\n", buf);

            if (strcmp(buf, "GET /favicon.ico") == 0) {
                int fdimg = open("favicon.ico", O_RDONLY);
                sendfile(fd_client, fdimg, NULL, 4000);
                close(fdimg);
            }
            else {
                write(fd_client, info, sizeof(info) - 1);
            }
            close(fd_client);
            printf("closing ... \n");
        }
        // parent process
        close(fd_client);
    }

    return EXIT_SUCCESS;
}
