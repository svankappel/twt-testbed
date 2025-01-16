/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef TEST_MULTI_PACKET_PS_H
#define TEST_MULTI_PACKET_PS_H

#include <zephyr/kernel.h>
#include "test_global.h"

/**
 * @brief Multi packet PS test
 *
 * This function runs the multi packet power save test.
 *
 * @param test_settings Pointer to the settings for the test.
 */
void test_multi_packet_ps(void * test_settings);

/**
 * @brief Settings for the multi packet power save test.
 *
 * This structure contains the settings used to configure the multi packet power save test.
 *
 * @param packet_number Number of packets to be sent every iteration.
 * @param test_id Identifier for the test.
 * @param iterations Number of iterations for the test.
 * @param ps_mode Power save mode to be tested.
 * @param ps_wakeup_mode Power save wakeup mode to be tested.
 */
struct test_multi_packet_ps_settings {
    uint16_t packet_number;
    uint8_t test_id;
    uint32_t iterations;
    uint8_t ps_mode;
    uint8_t ps_wakeup_mode;
};

#endif // TEST_MULTI_PACKET_PS_H

