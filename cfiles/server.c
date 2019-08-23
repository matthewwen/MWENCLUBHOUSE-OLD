#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <errno.h>
#include <sys/types.h>
#include <time.h>

#include <curl/curl.h>

// others
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define IPADDR "127.0.0.1"
#define PORTNUM 80

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}
 
static int closecb(void *clientp, curl_socket_t item)
{
  (void)clientp;
  printf("libcurl wants to close %d now\n", (int)item);
  return 0;
}
 
static curl_socket_t opensocket(void *clientp,
                                curlsocktype purpose,
                                struct curl_sockaddr *address)
{
  curl_socket_t sockfd;
  (void)purpose;
  (void)address;
  sockfd = *(curl_socket_t *)clientp;
  /* the actual externally set socket is passed in via the OPENSOCKETDATA
     option */ 
  return sockfd;
}
 
static int sockopt_callback(void *clientp, curl_socket_t curlfd,
                            curlsocktype purpose)
{
  (void)clientp;
  (void)curlfd;
  (void)purpose;
  /* This return code was added in libcurl 7.21.5 */ 
  return CURL_SOCKOPT_ALREADY_CONNECTED;
}
 
int main(void)
{
  CURL *curl;
  CURLcode res;
  struct sockaddr_in servaddr;  /*  socket address structure  */ 
  curl_socket_t sockfd;
 
  curl = curl_easy_init();
  if(curl) {
    /*
     * Note that libcurl will internally think that you connect to the host
     * and port that you specify in the URL option.
     */ 
    curl_easy_setopt(curl, CURLOPT_URL, "http://99.99.99.99:9999");
 
    /* Create the socket "manually" */ 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == CURL_SOCKET_BAD) {
      printf("Error creating listening socket.\n");
      return 3;
    }
 
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(PORTNUM);
 
    servaddr.sin_addr.s_addr = inet_addr(IPADDR);
    if(INADDR_NONE == servaddr.sin_addr.s_addr) {
      close(sockfd);
      return 2;
    }
 
    if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) ==
       -1) {
      close(sockfd);
      printf("client error: connect: %s\n", strerror(errno));
      return 1;
    }
 
    /* no progress meter please */ 
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
 
    /* send all data to this function  */ 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
 
    /* call this function to get a socket */ 
    curl_easy_setopt(curl, CURLOPT_OPENSOCKETFUNCTION, opensocket);
    curl_easy_setopt(curl, CURLOPT_OPENSOCKETDATA, &sockfd);
 
    /* call this function to close sockets */ 
    curl_easy_setopt(curl, CURLOPT_CLOSESOCKETFUNCTION, closecb);
    curl_easy_setopt(curl, CURLOPT_CLOSESOCKETDATA, &sockfd);
 
    /* call this function to set options for the socket */ 
    curl_easy_setopt(curl, CURLOPT_SOCKOPTFUNCTION, sockopt_callback);
 
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
 
    res = curl_easy_perform(curl);
 
    curl_easy_cleanup(curl);
 
    close(sockfd);
 
    if(res) {
      printf("libcurl error: %d\n", res);
      return 4;
    }
  }
 
  return 0;
}
