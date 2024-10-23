#ifndef COAP_H
#define COAP_H

#include <zephyr/kernel.h>


int coap_init();

int coap_validate();

int coap_put(const char *resource,uint8_t *payload, uint32_t timeout);

#endif