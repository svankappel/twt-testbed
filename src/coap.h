#ifndef COAP_H
#define COAP_H

#include <zephyr/kernel.h>

int server_resolve();

int coap_init();


int coap_put(const char *resource,uint8_t *payload,size_t len);

int coap_get(const char *resource);

int coap_observe(const char *resource, bool start_observe);

#endif