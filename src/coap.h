#ifndef COAP_H
#define COAP_H

#include <zephyr/kernel.h>


int coap_init();

int coap_put(const char *resource,uint8_t *payload, uint32_t timeout);

int coap_get(const char *resource, uint32_t timeout);

//int coap_observe(const char *resource, bool start_observe);

#endif