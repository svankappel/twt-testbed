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
#include "wifi/wifi_ps.h"
#include "wifi/wifi_twt.h"
#include "coap.h"
#include "profiler.h"
#include "tests/test_sensor_twt.h"


LOG_MODULE_REGISTER(main, CONFIG_MY_MAIN_LOG_LEVEL);

K_SEM_DEFINE(test_sem, 0, 1);


int main(void)
{
    int ret;

    LOG_INF("Starting TWT testbench ...");

    // initialize setup
    profiler_init();

    wifi_init();

    wifi_ps_disable();
    
    wifi_connect();
 

    coap_init();

    ret = coap_validate();
    if(ret != 0)
    {
        LOG_ERR("Failed to validate coap");
        k_sleep(K_FOREVER);
    }

    k_sleep(K_SECONDS(1));

    int i = 0;
    char buf[32];

    while(true)
    {
        snprintf(buf, sizeof(buf), "test-payload-%d", i++);
        coap_put("test/test1", buf, 1000);
        snprintf(buf, sizeof(buf), "test-payload-%d", i++);
        coap_put("test/test1", buf, 1000);

        k_sleep(K_SECONDS(5));
        
    }
}
