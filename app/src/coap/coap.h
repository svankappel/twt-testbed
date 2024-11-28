#ifndef COAP_H
#define COAP_H

#include <zephyr/kernel.h>


int coap_init();

int coap_validate();

int coap_get_stat();

int coap_observe(char *resource, uint8_t *payload);

int coap_cancel_observers();

void coap_register_put_response_callback(void (*callback)(uint32_t time, uint8_t * payload, uint16_t payload_len));
void coap_register_obs_response_callback(void (*callback)(uint8_t * payload, uint16_t payload_len));

int coap_put(char *resource,uint8_t *payload);

void coap_init_pool(uint32_t requests_timeout);


#endif