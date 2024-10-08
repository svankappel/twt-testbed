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

#include "wifi_sta.h"
#include "wifi_ps.h"
#include "coap.h"
#include "profiler.h"

LOG_MODULE_REGISTER(main, CONFIG_MY_MAIN_LOG_LEVEL);


int main(void)
{
	profiler_init();

	wifi_init();
	

	//coap_init(CONFIG_COAP_SAMPLE_SERVER_HOSTNAME,CONFIG_COAP_SAMPLE_SERVER_PORT);

	
	while(true)
	{

		wifi_ps_disable();
		wifi_connect();
		profiler_output_binary(0x1);
		k_sleep(K_SECONDS(10));
		profiler_output_binary(0x0);
		wifi_disconnect();

		wifi_ps_enable();
		wifi_ps_mode_legacy();
		wifi_ps_wakeup_dtim();
		wifi_connect();
		profiler_output_binary(0x2);
		k_sleep(K_SECONDS(10));
		profiler_output_binary(0x0);
		wifi_disconnect();

		wifi_ps_mode_wmm();
		wifi_connect();
		profiler_output_binary(0x3);
		k_sleep(K_SECONDS(10));
		profiler_output_binary(0x0);
		wifi_disconnect();

		wifi_ps_mode_legacy();
		wifi_ps_wakeup_listen_interval();
		wifi_connect();
		profiler_output_binary(0x4);
		k_sleep(K_SECONDS(10));
		profiler_output_binary(0x0);
		wifi_disconnect();

		wifi_ps_mode_wmm();
		wifi_ps_wakeup_listen_interval();
		wifi_connect();
		profiler_output_binary(0x5);
		k_sleep(K_SECONDS(10));
		profiler_output_binary(0x0);
		wifi_disconnect();
	}


	return 0;
}





