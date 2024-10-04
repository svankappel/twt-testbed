#ifndef WIFI_H
#define WIFI_H
#include <zephyr/net/wifi.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_mgmt.h>
#include <net/wifi_mgmt_ext.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

void wait_for_network(void);

int wifi_init();

int wifi_enable_ps();

int wifi_disable_ps();

#endif