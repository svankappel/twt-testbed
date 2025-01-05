#ifndef TEST_MULTI_PACKET_TWT_H
#define TEST_MULTI_PACKET_TWT_H

#include <zephyr/kernel.h>
#include "test_global.h"

void test_multi_packet_twt(void * test_settings);

struct test_multi_packet_twt_settings {
    uint16_t packet_number;
    uint32_t twt_interval;
    uint32_t twt_wake_interval;
    uint8_t test_id;
    uint32_t iterations;
    uint32_t wake_ahead_ms;
};

#endif // TEST_MULTI_PACKET_TWT_H
