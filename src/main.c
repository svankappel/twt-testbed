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
#include "tests/test_template.h"


LOG_MODULE_REGISTER(main, CONFIG_MY_MAIN_LOG_LEVEL);

K_SEM_DEFINE(test_sem, 1, 1);


int main(void)
{
	profiler_init();
	wifi_init();
	wifi_ps_disable();

        init_test_template(&test_sem);

        k_sleep(K_FOREVER);
	return 0;
}
