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

K_SEM_DEFINE(test_sem, 0, 1);


int main(void)
{
        // initialiye setup
	profiler_init();
	wifi_init();
	wifi_ps_disable();
        wifi_connect();
        coap_init();
        wifi_disconnect();
        k_sleep(K_SECONDS(1));

        // initialize the tests
        init_test_template(&test_sem);
        init_test_template(&test_sem);


        // give semaphore to start the tests
        k_sem_give(&test_sem);
        k_sleep(K_FOREVER);
	return 0;
}
