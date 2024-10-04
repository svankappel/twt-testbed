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

int wifi_init(void);

int wifi_ps_legacy_dtim(void);

int wifi_ps_wmm_dtim(void);

int wifi_ps_legacy_listen_interval(void);

int wifi_ps_wmm_listen_interval(void);

int wifi_ps_disable(void);

#endif // WIFI_H