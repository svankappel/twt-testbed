/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef TEST_SENSOR_PS_H
#define TEST_SENSOR_PS_H

#include <zephyr/kernel.h>
#include "test_global.h"

/**
 * @brief Sensor power save test
 *
 * This function runs the sensor power save test.
 *
 * @param test_settings Pointer to the settings for the test.
 */
void test_sensor_ps(void * test_settings);

/**
 * @brief Settings for the sensor power save test.
 *
 * This structure contains the settings used to configure the sensor power save test.
 *
 * @param send_interval Interval at which data is sent.
 * @param test_id Identifier for the test.
 * @param iterations Number of iterations for the test.
 * @param ps_mode Power save mode for the test.
 * @param ps_wakeup_mode Power save wakeup mode for the test.
 */
struct test_sensor_ps_settings {
    uint32_t send_interval;
    uint8_t test_id;
    uint32_t iterations;
    uint8_t ps_mode;
    uint8_t ps_wakeup_mode;
};

#endif // TEST_SENSOR_PS_H

