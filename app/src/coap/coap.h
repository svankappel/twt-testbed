#ifndef COAP_H
#define COAP_H

#include <zephyr/kernel.h>


int coap_init();

void coap_init_pool(uint32_t requests_timeout);


#ifdef CONFIG_COAP_TWT_TESTBED_SERVER
int coap_validate();
int coap_get_stat();
int coap_get_actuator_stat(char * buffer);
#endif //CONFIG_COAP_TWT_TESTBED_SERVER


int coap_put(char *resource,uint8_t *payload);

int coap_observe(char *resource, uint8_t *payload);
int coap_cancel_observe();

void coap_emergency_enable();
void coap_emergency_disable();

void coap_register_put_response_callback(void (*callback)(uint32_t time, uint8_t * payload, uint16_t payload_len));
void coap_register_obs_response_callback(void (*callback)(uint8_t * payload, uint16_t payload_len));





#endif