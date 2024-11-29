#ifndef TEST_MULTI_PACKET_PS_H
#define TEST_MULTI_PACKET_PS_H

#include <zephyr/kernel.h>
#include "test_global.h"

void test_multi_packet_ps(struct k_sem *sem, void * test_settings);

struct test_multi_packet_ps_settings {
    uint16_t packet_number;
    uint8_t test_id;
    uint32_t iterations;
    uint8_t ps_mode;
    uint8_t ps_wakeup_mode;
};

#endif // TEST_MULTI_PACKET_PS_H

