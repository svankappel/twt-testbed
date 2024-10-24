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
    ret = profiler_init();
    if(ret != 0)
    {
        LOG_ERR("Failed to initialize profiler");
        k_sleep(K_FOREVER);
    }

    wifi_init();

    ret = wifi_ps_disable();
    if(ret != 0)
    {
        LOG_ERR("Failed to disable power save mode");
        k_sleep(K_FOREVER);
    }

    ret = wifi_connect();
    if(ret != 0)
    {
        LOG_ERR("Failed to connect to wifi");
        k_sleep(K_FOREVER);
    }

    coap_init();
    if(ret != 0)
    {
        LOG_ERR("Failed to initialize CoAP client");
        k_sleep(K_FOREVER);
    }

    k_sleep(K_SECONDS(1));
    

    ret = coap_validate();
    if(ret != 0)
    {
        LOG_ERR("Failed to validate CoAP client");
        k_sleep(K_FOREVER);
    }

    //sleep some time to allow the AP to set up its ARP table
    LOG_INF("Waiting some time to allow the AP to set up its ARP table ...");
    k_sleep(K_SECONDS(10));
        
    ret = wifi_disconnect();
    if(ret != 0)
    {
        LOG_ERR("Failed to disconnect from wifi");
        k_sleep(K_FOREVER);
     }

    k_sleep(K_SECONDS(1));

    LOG_INF("TWT testbench initialized. Initializing tests ...");


    // initialize the tests

    struct test_sensor_twt_settings test_settings_1 = {
            .twt_interval = 5000,
            .twt_wake_interval = 10,
            .test_number = 1,
            .iterations = 10,
            .request_timeout = 6000
    };
    init_test_sensor_twt(&test_sem, &test_settings_1);


    LOG_INF("Tests initialized. Starting tests ...");


    // give semaphore to start the tests
    k_sem_give(&test_sem);
    k_sleep(K_FOREVER);
    return 0;
}
