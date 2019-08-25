#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <curl/curl.h>

static const int SIG_LEN = 75; 
static const int REQUEST_LEN = 2048;
   
int main(int argc, char const *argv[]) { 
    CURL * curl = curl_easy_init();
    if (curl != NULL) {
        struct curl_slist * chunk = NULL;

        chunk = curl_slist_append(chunk, "Accept:");
        chunk = curl_slist_append(chunk, "Service:Database");
        chunk = curl_slist_append(chunk, "Signature:");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_URL, "192.168.1.103:8080");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            printf("error!!\n");
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(chunk);
    }
    return EXIT_SUCCESS;
} 