/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef TEST_LARGE_PACKET_PS_H
#define TEST_LARGE_PACKET_PS_H

#include <zephyr/kernel.h>
#include "test_global.h"


/**
 * @brief Large packet PS test
 *
 * This function runs the large packet power save test.
 *
 * @param test_settings Pointer to the settings for the test.
 */
void test_large_packet_ps(void * test_settings);

/**
 * @brief Settings for the large packet power save test.
 *
 * This structure contains the settings used to configure the large packet power save test.
 *
 * @param send_interval Interval between sending packets.
 * @param test_id Identifier for the test.
 * @param large_packet_config Configuration of large/small request/response (only applicable if CONFIG_COAP_TWT_TESTBED_SERVER is defined).
 * @param bytes size of the payload.
 * @param iterations Number of iterations for the test.
 * @param ps_mode Power save mode to be tested.
 * @param ps_wakeup_mode Power save wakeup mode to be tested.
 */
struct test_large_packet_ps_settings {
    uint32_t send_interval;
    uint8_t test_id;
    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
    uint8_t large_packet_config; // LREQ_SREP, SREQ_LREP, or LREQ_LREP
    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
    uint32_t bytes;
    uint32_t iterations;
    uint8_t ps_mode;
    uint8_t ps_wakeup_mode;
};

#endif // TEST_LARGE_PACKET_PS_H

