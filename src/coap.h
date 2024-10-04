#ifndef COAP_H
#define COAP_H

#include <zephyr/net/socket.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/net/coap.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/conn_mgr_connectivity.h>
#include <zephyr/net/conn_mgr_monitor.h>
#include <zephyr/random/random.h>
#include <zephyr/net/coap_client.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>



int server_resolve(struct sockaddr_storage *server,const char* hostname,int port);

void response_cb(int16_t code, size_t offset, const uint8_t *payload,size_t len, bool last_block, void *user_data);

int coap_init(const char* host,int port);

int coap_put(const char *resource,uint8_t *payload,size_t len);

int coap_get(const char *resource);

#endif