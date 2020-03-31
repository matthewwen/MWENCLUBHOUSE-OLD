#ifndef _MQTT_HANDLER_
#define _MQTT_HANDLER_

#include <stdio.h>
#include <stdlib.h>
#include <czmq.h>
#include <zcert.h>

#define MQTT_MAX_LENGTH 1000000

void handle_mqtt_request(int n, void * context, char * buffer);
void set_mqtt_response(void * responder);

#endif
