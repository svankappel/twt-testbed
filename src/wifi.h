#ifndef WIFI_H
#define WIFI_H
#include <zephyr/net/wifi.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_mgmt.h>
#include <net/wifi_mgmt_ext.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

int wifi_args_to_params(struct wifi_connect_req_params *params);

void wait_for_network(void);

void l4_event_handler(struct net_mgmt_event_callback *cb,uint32_t event,struct net_if *iface);

int wifi_init();

int wifi_enable_ps();

int wifi_disable_ps();

#endif