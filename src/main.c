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
#include <zephyr/random/random.h>

#include "wifi_sta.h"
#include "wifi_ps.h"
#include "coap.h"
#include "profiler.h"

LOG_MODULE_REGISTER(main, CONFIG_MY_MAIN_LOG_LEVEL);


int main(void)
{
	profiler_init();

	wifi_init();

	wifi_ps_mode_legacy();
	wifi_ps_set_listen_interval(10);
	wifi_ps_wakeup_listen_interval();
	wifi_ps_disable();

	wifi_connect();

	coap_init();

	coap_observe("obs", true);

	while(true)
	{
		//power save
		profiler_ch0_set();
		wifi_ps_enable();

		for(int i = 0; i < 5; i++)
		{
			uint8_t message[25];
			sprintf(message,"{\"sensor-value\":%d}",sys_rand8_get());	
			coap_put("validate", "message", strlen(message));
			k_sleep(K_SECONDS(4));
		}

		//no power save
		profiler_ch0_clear();
		wifi_ps_disable();

		for(int i = 0; i < 5; i++)
		{
			uint8_t message[25];
			sprintf(message,"{\"sensor-value\":%d}",sys_rand8_get());	
			coap_put("validate", "message", strlen(message));
			k_sleep(K_SECONDS(4));
		}
	}


	return 0;
}





