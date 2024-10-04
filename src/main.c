/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include "wifi.h"
#include "coap.h"
#include "profiler.h"

LOG_MODULE_REGISTER(main, CONFIG_MY_MAIN_LOG_LEVEL);


int main(void)
{
	int err;
	profiler_init();
	wifi_init();

	err = conn_mgr_all_if_connect(true);
	if (err) {
		LOG_ERR("conn_mgr_all_if_connect, error: %d", err);
		return err;
	}

	wait_for_network();

	LOG_INF("Network connected");

	k_sleep(K_FOREVER);

	
	coap_init(CONFIG_COAP_SAMPLE_SERVER_HOSTNAME,CONFIG_COAP_SAMPLE_SERVER_PORT);

	while(true)
	{
		uint8_t message[25];
		sprintf(message,"{\"random-value\":%d}",sys_rand8_get());

		coap_put(CONFIG_COAP_SAMPLE_PUT_RESOURCE,message,strlen(message));
		k_sleep(K_SECONDS(3));

		coap_get(CONFIG_COAP_SAMPLE_GET_RESOURCE);
		k_sleep(K_SECONDS(3));
	}
	

	return 0;
}
