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
K_THREAD_STACK_DEFINE(test_thread_stack, 4096);

K_SEM_DEFINE(test_sem, 0, 1);

int d;
struct k_thread debug_thread_data;

void coap_debug_thread(void)
{
    k_sem_take(&test_sem, K_FOREVER);

    wifi_connect();

    d = 0;
    char buf[32];
    snprintf(buf, sizeof(buf), "payload");

    while (true)
    {
        snprintf(buf, sizeof(buf), "test-payload-%d", d++);
        coap_put("test/test1", buf, 1000);
        snprintf(buf, sizeof(buf), "test-payload-%d", d++);
        coap_put("test/test1", buf, 1000);

        k_sleep(K_SECONDS(5));
    }
}

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

    k_sleep(K_SECONDS(1));

    
    ret = coap_validate();
    if (ret != 0)
    {
        LOG_ERR("Failed to validate coap");
        k_sleep(K_FOREVER);
    }

    wifi_disconnect();

    k_sleep(K_SECONDS(5));


    k_tid_t test_thread_id = k_thread_create(&debug_thread_data, test_thread_stack ,4096, coap_debug_thread, NULL, NULL, NULL, 7, 0, K_NO_WAIT);
    k_thread_name_set(test_thread_id, "test_thread");
    k_thread_start(test_thread_id);

    k_sem_give(&test_sem);

    k_sleep(K_FOREVER);
}
