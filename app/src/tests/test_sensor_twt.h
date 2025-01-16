/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#ifndef TEST_SENSOR_TWT_H
#define TEST_SENSOR_TWT_H

#include <zephyr/kernel.h>
#include "test_global.h"

/**
 * @brief Sensor TWT (Target Wake Time) test
 *
 * This function runs the sensor TWT test.
 *
 * @param test_settings Pointer to the settings for the test.
 */
void test_sensor_twt(void * test_settings);

/**
 * @brief Settings for the sensor TWT (Target Wake Time) test.
 *
 * This structure contains the settings used to configure the sensor TWT test.
 *
 * @param twt_interval Interval for TWT.
 * @param twt_wake_interval Wake interval for TWT.
 * @param test_id Identifier for the test.
 * @param iterations Number of iterations for the test.
 * @param wake_ahead_ms Time in milliseconds to wake up ahead of the TWT session
 * @param recover Enable recovery mode.
 * @param recover_max_pending Pending request threshold for recovery.
 */
struct test_sensor_twt_settings {
    uint32_t twt_interval;
    uint32_t twt_wake_interval;
    uint8_t test_id;
    uint32_t iterations;
    uint32_t wake_ahead_ms;
    bool recover;
    uint8_t recover_max_pending;
};

#endif // TEST_SENSOR_TWT_H
