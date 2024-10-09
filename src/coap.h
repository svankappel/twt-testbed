#ifndef COAP_H
#define COAP_H

#include <zephyr/net/socket.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/net/socket.h>
#include <zephyr/random/random.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/net/tls_credentials.h>


int server_resolve();

//void response_cb(int16_t code, size_t offset, const uint8_t *payload,size_t len, bool last_block, void *user_data);

int coap_init(const char* host,int port);

/*
int coap_put(const char *resource,uint8_t *payload,size_t len);

int coap_get(const char *resource);
*/
#endif