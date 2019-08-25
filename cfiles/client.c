#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <curl/curl.h>
#include <sys/stat.h>
#include <unistd.h>

static const int SIG_LEN     = 75; 
static const int REQUEST_LEN = 2048;

void get_request();
void post_request();
void put_request();

int count = 0;

int main(int argc, char const *argv[]) { 
    //get_request();
    curl_global_init(CURL_GLOBAL_ALL);
    put_request();
    curl_global_cleanup();
    return EXIT_SUCCESS;
} 

void get_request() {
    CURL * curl = curl_easy_init();
    if (curl != NULL) {
        struct curl_slist * chunk = NULL;

        chunk = curl_slist_append(chunk, "Accept:");
        chunk = curl_slist_append(chunk, "Service: Database");
        chunk = curl_slist_append(chunk, "Signature: getrequest");
        chunk = curl_slist_append(chunk, "Content-Type: application/json");

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
}

void post_request() {
    CURL * curl = curl_easy_init();
    if (curl != NULL) {
        struct curl_slist * chunk = NULL;

        chunk = curl_slist_append(chunk, "Accept:");
        chunk = curl_slist_append(chunk, "Service:Database");
        chunk = curl_slist_append(chunk, "Signature:postrequest");
        chunk = curl_slist_append(chunk, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_URL, "192.168.1.103:8080");
        //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"Hello\": 5}");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        FILE * fp = fopen("j.json", "r");
        curl_easy_setopt(curl, CURLOPT_READDATA, fp);
        fclose(fp);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            printf("error!!\n");
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(chunk);

    }
}

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    //printf("num member: %ld\n", nmemb);
    nmemb = 999;
    size_t retcode   = fread(ptr, size, nmemb, stream);
    curl_off_t nread = (curl_off_t) retcode;

    fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
            " bytes from file\n", nread);

    return retcode;
}

static size_t write_callback(void * contents, size_t size, size_t nmem, void * userp) {
    size_t realsize = size * nmem;
    char * msg = malloc(realsize * sizeof(*msg));
    memcpy(msg, contents, realsize);
    printf("response:%s\n", msg);
    free(msg);

    char ** fmsg = (char **) userp;
    *fmsg = "From the Function";
    return realsize;
}

void put_request() {
    CURL * curl = curl_easy_init();
    FILE * fp = fopen("j.json", "r");
    fseek(fp, 0, SEEK_SET);
    struct stat file_info;
    stat("j.json", &file_info);
    char * response = NULL;
    if (curl != NULL) {
        struct curl_slist * chunk = NULL;

        chunk = curl_slist_append(chunk, "Accept:");
        // chunk = curl_slist_append(chunk, "Expect:");
        chunk = curl_slist_append(chunk, "Service: Database");
        chunk = curl_slist_append(chunk, "Signature: postrequest");
        chunk = curl_slist_append(chunk, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        curl_easy_setopt(curl, CURLOPT_READDATA, fp);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_PUT, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, "192.168.1.103:8080");
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) file_info.st_size);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        CURLcode res = !CURLE_OK;
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            printf("error: %s\n", curl_easy_strerror(res));
        }

        if (response != NULL) {
            printf("function:%s\n", response);
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(chunk);
    }
    fclose(fp);
}
