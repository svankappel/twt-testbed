/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef TEST_LARGE_PACKET_TWT_H
#define TEST_LARGE_PACKET_TWT_H

#include <zephyr/kernel.h>
#include "test_global.h"


/**
 * @brief Large Packet TWT test
 *
 * This function runs the large packet Target Wake Time (TWT) test.
 *
 * @param test_settings Pointer to the settings for the test.
 */
void test_large_packet_twt(void * test_settings);

/**
 * @brief Settings for the large packet TWT test.
 *
 * This structure contains the settings used to configure the large packet TWT test.
 *
 * @param twt_interval Interval for TWT
 * @param twt_wake_interval Wake interval for TWT
 * @param large_packet_config Configuration for small/large request/responses (only applicable if CONFIG_COAP_TWT_TESTBED_SERVER is defined).
 * @param bytes size of the payload.
 * @param test_id Identifier for the test.
 * @param iterations Number of iterations for the test.
 * @param wake_ahead_ms Time in milliseconds to wake ahead.
 */
struct test_large_packet_twt_settings {
    uint32_t twt_interval;
    uint32_t twt_wake_interval;

    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
    uint8_t large_packet_config; // LREQ_SREP, SREQ_LREP, or LREQ_LREP
    #endif //CONFIG_COAP_TWT_TESTBED_SERVER

    uint32_t bytes;
    uint8_t test_id;
    uint32_t iterations;
    uint32_t wake_ahead_ms;
};

#endif // TEST_LARGE_PACKET_TWT_H
