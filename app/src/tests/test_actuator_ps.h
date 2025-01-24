/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef TEST_ACTUATOR_PS_H
#define TEST_ACTUATOR_PS_H

#include <zephyr/kernel.h>
#include "test_global.h"

/**
 * @brief Actuator PS test
 *
 * This function runs the actuator power save test.
 *
 * @param test_settings Pointer to the settings for the test.
 */
void test_actuator_ps(void * test_settings);

/**
 * @brief Settings for the actuator power save test.
 *
 * This structure contains the settings used to configure the actuator power save test.
 *
 * @param test_id identifier for the test.
 * @param test_time_s Duration of the test (seconds)
 * @param min_interval Minimum interval between notifications (seconds) (only applicable if CONFIG_COAP_TWT_TESTBED_SERVER is defined).
 * @param max_interval Maximum interval between notifications (seconds) (only applicable if CONFIG_COAP_TWT_TESTBED_SERVER is defined).
 * @param echo Enable or disable echo functionality (only applicable if CONFIG_COAP_TWT_TESTBED_SERVER is defined).
 * @param ps_mode Power save mode to be tested.
 * @param ps_wakeup_mode Power save wakeup mode to be tested.
 */
struct test_actuator_ps_settings {
    uint8_t test_id;
    uint32_t test_time_s;
    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
    uint32_t min_interval;
    uint32_t max_interval;
    bool echo;
    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
    uint8_t ps_mode;
    uint8_t ps_wakeup_mode;
};

#endif // TEST_ACTUATOR_PS_H

