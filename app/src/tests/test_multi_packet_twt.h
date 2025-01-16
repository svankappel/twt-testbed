/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef TEST_MULTI_PACKET_TWT_H
#define TEST_MULTI_PACKET_TWT_H

#include <zephyr/kernel.h>
#include "test_global.h"

/**
 * @brief Multi packet TWT test
 *
 * This function runs the multi packet TWT test.
 *
 * @param test_settings Pointer to the settings for the test.
 */
void test_multi_packet_twt(void * test_settings);

/**
 * @brief Settings for the multi packet TWT test.
 *
 * This structure contains the settings used to configure the multi packet TWT test.
 *
 * @param packet_number Number of packets to be sent every iteration.
 * @param twt_interval TWT interval for the test.
 * @param twt_wake_interval TWT wake interval for the test.
 * @param test_id Identifier for the test.
 * @param iterations Number of iterations for the test.
 * @param wake_ahead_ms Wake ahead time in milliseconds.
 */
struct test_multi_packet_twt_settings {
    uint16_t packet_number;
    uint32_t twt_interval;
    uint32_t twt_wake_interval;
    uint8_t test_id;
    uint32_t iterations;
    uint32_t wake_ahead_ms;
};

#endif // TEST_MULTI_PACKET_TWT_H
