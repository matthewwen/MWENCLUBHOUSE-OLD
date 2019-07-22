#include <stdio.h>
#include <stdlib.h>
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

#define SERVER_PORT 18000


int main(int argc, char * argv[]) {

    int server_fd = socket(AF_INET, SOCK_RAW,  
    printf("Hello world\n");
    return EXIT_SUCCESS;
}
