#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mqtthandler.h"

void handle_mqtt_request(int n, void * context, void * responder, char * buffer) {
    bool is_valid = n != -1 && n < MQTT_MAX_LENGTH;

    if (is_valid) {
        printf("buffer: %s\n", buffer);
        memset(buffer, 0, n + 1);
		zmq_send(responder, "World", 5, 0);
    }
}
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
