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

	LOG_INF("Network connected");

	k_sleep(K_FOREVER);

	//coap_init(CONFIG_COAP_SAMPLE_SERVER_HOSTNAME,CONFIG_COAP_SAMPLE_SERVER_PORT);
/*
	wifi_ps_enable();
	k_sleep(K_SECONDS(1));
	wifi_ps_wakeup_listen_interval();

	while(true)
	{

		wifi_ps_wakeup_listen_interval();
		k_sleep(K_MSEC(5000));
		int wifi_ps_wakeup_dtim();
		k_sleep(K_MSEC(5000));
	}
*/

	return 0;
}





