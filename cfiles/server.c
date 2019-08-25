#include <unistd.h> 
#include <stdio.h> 
#include <stdbool.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <assert.h>
#define PORT 8080 
#define SIZE 1000

int create_server_socket(struct sockaddr_in * address) {
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

    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)address, sizeof(*address))<0) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 5) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    return server_fd;
}

int main(int argc, char const *argv[]) { 
    struct sockaddr_in address = {.sin_family = AF_INET, 
                                  .sin_addr   = {.s_addr = INADDR_ANY}, 
                                  .sin_port   = htons(PORT)};
    int addrlen = sizeof(address); 
    int server_fd = create_server_socket(&address);
       

    char buffer[SIZE] = {0}; 
    char * response = "Reponse Message"; 
    for (int new_socket = -1; true; new_socket = -1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) >= 0) { 
            printf("\n--------------header-----------------\n");
            for (int valread = SIZE - 1; valread == SIZE - 1; ) {
                valread = read(new_socket, buffer, SIZE - 1);
                buffer[valread + 1] = '\0';
                printf("%s", buffer);
            }

            printf("\n--------------data-----------------\n");
            for (int valread = SIZE - 1; valread == SIZE - 1; ) {
                valread = read(new_socket, buffer, SIZE - 1);
                buffer[valread + 1] = '\0';
                printf("%s", buffer);
            }
            printf("\n------------------\n");

            write(new_socket, response, strlen(response) + 1); 
            close(new_socket);
        } 
    }
    return 0; 
} 