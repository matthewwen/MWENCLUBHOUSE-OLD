#include "header.h"

#define PORT 80
#define NUMBER_OF_THREADS 2

#define REQUEST_REFUSED_PAGE \
	"<html><head><title>Request refused</title></head><body>Request refused</body></html>"
#define OKAY_RETURN \
	"200"

static const char HTML_DIR[] = "/www/";
static const char JAVASCRIPT_DIR[] = "/www/js/";
static const char USER_DIR[] = "/usr/";
static char * INDEX_HTML = NULL;

typedef int request_type_t;
enum {NONE = 0, PUT = 1, POST = 2, GET = 3, HEAD = 4};

typedef struct{
	struct MHD_PostProcessor *pp; 
	struct MHD_Connection *connection; 
	struct MHD_Response *response;  
	bool used_function;
}UploadContext;

static struct MHD_Response * CACHED_DIRECTORY_RESPONSE = NULL; 
static struct MHD_Response * OKAY_RESPONSE             = NULL; 
static struct MHD_Response * REQUEST_REFUSED_RESPONSE  = NULL; 
static pthread_mutex_t MUTEX; 

void catcher(int sig) {(void) sig;}

int process_upload_data (void *cls, 
                        enum MHD_ValueKind kind, 
						const char *key, 
						const char *filename, 
						const char *content_type, 
						const char *transfer_encoding, 
						const char *data, 
						uint64_t off, 
						size_t size) {
	UploadContext * cxt = cls;
	cxt->used_function = true;

	return MHD_YES;
}

int generate_page (void *cls, 
                   struct MHD_Connection *connection, 
                   const char *url, 
		   const char *method, 
                   const char *version, 
                   const char *upload_data, 
                   size_t *upload_data_size, 
                   void **ptr) {

	int ret = MHD_NO;

	request_type_t request; 
	if ( (request = ( (0 == strcmp(method, MHD_HTTP_METHOD_GET)) ? GET : (0 == strcmp(method, MHD_HTTP_METHOD_HEAD)) ? HEAD: NONE)) != NONE) {
		if (0 == strcmp(url, "/")) {
			pthread_mutex_lock(&MUTEX);
			return ret;
		}
		if (request == GET) {
			return handle_get_request(url, connection, REQUEST_REFUSED_RESPONSE);
		}
	}
	else if ( (request = (0 == strcmp(method, MHD_HTTP_METHOD_PUT)) ? PUT : (0 == strcmp(method, MHD_HTTP_METHOD_POST)) ? POST: NONE) != NONE ) {
		UploadContext * uc = *ptr;

		if (uc == NULL) {
			uc = malloc(sizeof(*uc));
			memset(uc, 0, sizeof(*uc));

			uc->connection = connection;
			uc->pp = MHD_create_post_processor(connection, 64 * 1024, &process_upload_data, uc); // 64 * 1024

			*ptr = uc;
			return MHD_YES;
		}
		if (*upload_data_size != 0) {
			if (uc->response == NULL) {
				(void) MHD_post_process(uc->pp, upload_data, *upload_data_size);
			}
			if (uc->used_function == false) {
				if (request == PUT) {
					uc->response = handle_put_request(url, upload_data, upload_data_size);
				}
				else if (request == POST) {

				}
			}
			*upload_data_size = 0;
			return MHD_YES;
		}

		MHD_destroy_post_processor(uc->pp);
		uc->pp = NULL;

		// respond back 
		pthread_mutex_lock(&MUTEX);
		response = MHD_create_response_from_buffer(sizeof(OKAY_RETURN), (void * ) OKAY_RETURN, MHD_RESPMEM_PERSISTENT);
		ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
		MHD_destroy_response(response);
		pthread_mutex_unlock(&MUTEX);


		return ret;
	}

	return MHD_queue_response(connection, MHD_HTTP_OK, REQUEST_REFUSED_RESPONSE);
}

void response_completed_callback (void *cls, 
                                  struct MHD_Connection *connection, 
                                  void ** con_cls, 
                                  enum MHD_RequestTerminationCode toe) {
	UploadContext * uc = (UploadContext *) (*con_cls);

	// method to free UploadContext
	if (uc != NULL) {
		if (uc->pp != NULL) {
			MHD_destroy_post_processor(uc->pp);
		}
		if (uc->response != NULL) {
			MHD_destroy_response(uc->response);
		}
		free(uc);
	}
}

int main(int argc, char * argv[]) {

	// sig action pipline
	struct sigaction oldsig;
	memset(&oldsig, 0, sizeof(oldsig));
	struct sigaction sig;
	memset(&sig, 0, sizeof(sig));
	sig.sa_handler = &catcher;
	sig.sa_flags   = SA_INTERRUPT;
	if (0 != sigaction(SIGPIPE, &sig, &oldsig)) {
		fprintf(stderr, "Failed to Ignore sigipe: %s\n", strerror(errno));
	}

	// pthread
	(void) pthread_mutex_init(&MUTEX, NULL);

	REQUEST_REFUSED_RESPONSE = MHD_create_response_from_buffer(strlen(REQUEST_REFUSED_PAGE), (void * ) REQUEST_REFUSED_PAGE, MHD_RESPMEM_PERSISTENT);
	OKAY_RESPONSE = MHD_create_response_from_buffer(strlen(OKAY_RETURN), (void * ) OKAY_RETURN, MHD_RESPMEM_PERSISTENT);

    	size_t file_size = get_file(&INDEX_HTML, "www/index.html");
	CACHED_DIRECTORY_RESPONSE = MHD_create_response_from_buffer(file_size, (void *) INDEX_HTML, MHD_RESPMEM_PERSISTENT);

	// start up web server
	struct MHD_Daemon * d;
	d = MHD_start_daemon (MHD_USE_AUTO | MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_ERROR_LOG, PORT, NULL, NULL, 
	                      &generate_page, NULL, MHD_OPTION_CONNECTION_MEMORY_LIMIT, 
						  (size_t) (256 * 1024), MHD_OPTION_PER_IP_CONNECTION_LIMIT, 
						  (unsigned int) (64), MHD_OPTION_CONNECTION_TIMEOUT, 
						  (unsigned int) (120 /* seconds */), MHD_OPTION_THREAD_POOL_SIZE, 
						  (unsigned int) NUMBER_OF_THREADS, MHD_OPTION_NOTIFY_COMPLETED, 
						  &response_completed_callback, NULL, MHD_OPTION_END);
    

	// continously run
	while (true) {}
	
	// closing method
	(void) pthread_mutex_destroy(&MUTEX);
	MHD_stop_daemon(d);
	MHD_destroy_response(REQUEST_REFUSED_RESPONSE);
	MHD_destroy_response(CACHED_DIRECTORY_RESPONSE);
	free(INDEX_HTML);

	return EXIT_SUCCESS;
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
