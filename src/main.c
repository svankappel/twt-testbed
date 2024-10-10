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
	wifi_connect();
	

	coap_init();


	while(true)
	{
		uint8_t message[25];
		sprintf(message,"{\"sensor-value\":%d}",sys_rand8_get());
		coap_put("validate",message,strlen(message));
		k_sleep(K_SECONDS(5));
		coap_get("validate");
		k_sleep(K_SECONDS(5));
	}


	return 0;
}





