#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "mqtthandler.h"

// GLOBAL VARIABLE 
static void * MQTT_RESPONSE = NULL;

void handle_mqtt_request(int n, void * context, char * buffer) {
    assert(MQTT_RESPONSE != NULL);
    bool is_valid = n != -1 && n < MQTT_MAX_LENGTH && MQTT_RESPONSE != NULL;

    if (is_valid) {
        memset(buffer, 0, n + 1);
		zmq_send(MQTT_RESPONSE, "World", 5, ZMQ_DONTWAIT);
    }
}

void set_mqtt_response(void * responder) {
    MQTT_RESPONSE = responder;
}

void send_mqtt_msg(char * buffer) {
    int len = strlen(buffer);
    if (len < MQTT_MAX_LENGTH) {
        zmq_send(MQTT_RESPONSE, buffer, len, ZMQ_DONTWAIT);
    }
}

/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
