#ifndef COAP_H
#define COAP_H

#include <zephyr/kernel.h>


int coap_init();

int coap_validate();

int coap_get_stat();

void coap_register_response_callback(void (*callback)(int16_t code, void * user_data),void * callback_user_data);

int coap_put(const char *resource,uint8_t *payload, uint32_t timeout);


#endif