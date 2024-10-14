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

#include "wifi/wifi_sta.h"
#include "coap.h"
#include "profiler.h"


LOG_MODULE_REGISTER(main, CONFIG_MY_MAIN_LOG_LEVEL);


int main(void)
{
	profiler_init();

	wifi_init();

	wifi_ps_disable();

	wifi_connect();

	while(true)
	{
		wifi_twt_setup(50, 1000);
		k_sleep(K_SECONDS(20));
		wifi_twt_teardown();
		k_sleep(K_SECONDS(20));
	}


	return 0;
}
