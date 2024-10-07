#ifndef WIFI_H
#define WIFI_H

#include <zephyr/logging/log.h>
#include <nrfx_clock.h>
#include <zephyr/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/shell/shell.h>
#include <zephyr/init.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>

#include <zephyr/net/net_core.h>

#include <net/wifi_mgmt_ext.h>
#include <net/wifi_ready.h>

//#include <qspi_if.h>

//#include "net_private.h"


int wifi_init(void);
/*
int wifi_ps_legacy_dtim(void);

int wifi_ps_wmm_dtim(void);

int wifi_ps_legacy_listen_interval(int interval);

int wifi_ps_wmm_listen_interval(int interval);

int wifi_ps_disable(void);
*/
#endif // WIFI_H