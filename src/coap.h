#ifndef COAP_H
#define COAP_H

#include <zephyr/kernel.h>

int server_resolve();

//void response_cb(int16_t code, size_t offset, const uint8_t *payload,size_t len, bool last_block, void *user_data);

int coap_init();


int coap_put(const char *resource,uint8_t *payload,size_t len);

int coap_get(const char *resource);

#endif