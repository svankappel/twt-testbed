/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef TEST_ACTUATOR_TWT_H
#define TEST_ACTUATOR_TWT_H

#include <zephyr/kernel.h>
#include "test_global.h"

/**
 * @brief Actuator TWT test
 *
 * This function runs the actuator Target Wake Time (TWT) test.
 *
 * @param test_settings Pointer to the settings for the test.
 */
void test_actuator_twt(void * test_settings);

/**
 * @brief Settings for the actuator TWT test.
 *
 * This structure contains the settings used to configure the actuator TWT test.
 *
 * @param twt_interval Interval for TWT
 * @param twt_wake_interval Wake interval for TWT
 * @param test_id Identifier for the test.
 * @param min_interval Minimum interval between notifications (only applicable if CONFIG_COAP_TWT_TESTBED_SERVER is defined).
 * @param max_interval Maximum interval between notifications (only applicable if CONFIG_COAP_TWT_TESTBED_SERVER is defined).
 * @param echo Enable or disable echo functionality (only applicable if CONFIG_COAP_TWT_TESTBED_SERVER is defined).
 * @param test_time_s Duration of the test in seconds.
 * @param emergency_uplink Enable or disable emergency for uplink transmissions.
 */
struct test_actuator_twt_settings {
    uint32_t twt_interval;
    uint32_t twt_wake_interval;
    uint8_t test_id;

    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
    uint32_t min_interval;
    uint32_t max_interval;
    bool echo;
    #endif //CONFIG_COAP_TWT_TESTBED_SERVER

    uint32_t test_time_s;
    bool emergency_uplink;
};

#endif // TEST_ACTUATOR_TWT_H
