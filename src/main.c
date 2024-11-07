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
#include "wifi_twt.h"

#include "coap.h"

#include "profiler.h"

#include "test_sensor_twt.h"
#include "test_sensor_ps.h"


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

    wifi_ps_set_listen_interval(CONFIG_PS_LISTEN_INTERVAL);

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
        
    ret = wifi_disconnect();
    if(ret != 0)
    {
        LOG_ERR("Failed to disconnect from wifi");
        k_sleep(K_FOREVER);
     }

    k_sleep(K_SECONDS(1));

    LOG_INF("TWT testbench initialized. Running tests ...");


    // initialize the tests
    struct test_sensor_ps_settings test_settings_1 = {
            .iterations = 5,
            .send_interval = 5000,
            .test_number = 1,
            .ps_enabled = PS_MODE_DISABLED,
            .ps_mode = PS_MODE_LEGACY,
            .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM
    };
    test_sensor_ps(&test_sem, &test_settings_1);

    struct test_sensor_ps_settings test_settings_2 = {
            .iterations = 5,
            .send_interval = 5000,
            .test_number = 2,
            .ps_enabled = PS_MODE_ENABLED,
            .ps_mode = PS_MODE_LEGACY,
            .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM
    };
    test_sensor_ps(&test_sem, &test_settings_2);

    struct test_sensor_ps_settings test_settings_3 = {
            .iterations = 5,
            .send_interval = 5000,
            .test_number = 3,
            .ps_enabled = PS_MODE_ENABLED,
            .ps_mode = PS_MODE_LEGACY,
            .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL
    };
    test_sensor_ps(&test_sem, &test_settings_3);

    struct test_sensor_ps_settings test_settings_4 = {
            .iterations = 5,
            .send_interval = 5000,
            .test_number = 4,
            .ps_enabled = PS_MODE_ENABLED,
            .ps_mode = PS_MODE_WMM,
            .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM
    };
    test_sensor_ps(&test_sem, &test_settings_4);

    struct test_sensor_ps_settings test_settings_5 = {
            .iterations = 5,
            .send_interval = 5000,
            .test_number = 5,
            .ps_enabled = PS_MODE_ENABLED,
            .ps_mode = PS_MODE_WMM,
            .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL
    };
    test_sensor_ps(&test_sem, &test_settings_5);

/*
    struct test_sensor_twt_settings test_settings_1 = {
            .twt_interval = 20000,
            .twt_wake_interval = 8,
            .test_number = 1,
            .iterations = 5000,
            .wake_ahead_ms = 100
    };
    test_sensor_twt(&test_sem, &test_settings_1);

    
    struct test_sensor_twt_settings test_settings_2 = {
            .twt_interval = 20000,
            .twt_wake_interval = 8,
            .test_number = 2,
            .iterations = 5000,
            .wake_ahead_ms = 100
    };
    test_sensor_twt(&test_sem, &test_settings_2);
*/


    LOG_INF("Tests Finished!");


    k_sleep(K_FOREVER);
    return 0;
}
