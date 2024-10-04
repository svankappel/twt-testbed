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

	coap_init(CONFIG_COAP_SAMPLE_SERVER_HOSTNAME,CONFIG_COAP_SAMPLE_SERVER_PORT);

	while(true)
	{
		// Test each power save mode for 20 seconds
		for (int mode = 0; mode < 5; mode++) {
			switch (mode) {
				case 0:
					LOG_INF("Testing wifi_ps_disable");
					profiler_output_binary(0x00);
					wifi_ps_disable();
					break;
				case 1:
					LOG_INF("Testing wifi_ps_legacy_dtim");
					profiler_output_binary(0x01);
					wifi_ps_legacy_dtim();
					break;
				case 2:
					LOG_INF("Testing wifi_ps_wmm_dtim");
					profiler_output_binary(0x02);
					wifi_ps_wmm_dtim();
					break;
				case 3:
					LOG_INF("Testing wifi_ps_legacy_listen_interval");
					profiler_output_binary(0x04);
					wifi_ps_legacy_listen_interval();
					break;
				case 4:
					LOG_INF("Testing wifi_ps_wmm_listen_interval");
					profiler_output_binary(0x08);
					wifi_ps_wmm_listen_interval();
					break;
			}

			// Send sensor value to server every 5 seconds for 20 seconds
			for (int i = 0; i < 4; i++) {
				uint8_t message[25];
				sprintf(message, "{\"sensor-value\":%d}", sys_rand8_get());
				coap_put(CONFIG_COAP_SAMPLE_PUT_RESOURCE, message, strlen(message));
				k_sleep(K_SECONDS(5));
			}
		}
	}
	

	return 0;
}
