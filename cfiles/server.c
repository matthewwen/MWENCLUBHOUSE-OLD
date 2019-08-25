#include <unistd.h> 
#include <stdio.h> 
#include <stdbool.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 8080 
#define SIZE 2000

int main(int argc, char const *argv[]) { 
       
    // Creating socket file descriptor 
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    int opt = 1; 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 

    struct sockaddr_in address = {.sin_family = AF_INET, 
                                  .sin_addr   = {.s_addr = INADDR_ANY}, 
                                  .sin_port   = htons(PORT)};
    int addrlen = sizeof(address); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    char buffer[SIZE] = {0}; 
    char * hello = "<html>Hello from server</html>"; 
    for (int new_socket = -1; true; new_socket = -1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) >= 0) { 
            int valread = read( new_socket , buffer, SIZE); 
            printf("%s\n",buffer ); 
            send(new_socket , hello , strlen(hello) , 0 ); 
            printf("Hello message sent\n"); 
            close(new_socket);
        } 
    }
    return 0; 
} 