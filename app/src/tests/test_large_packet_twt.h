#ifndef TEST_LARGE_PACKET_TWT_H
#define TEST_LARGE_PACKET_TWT_H

#include <zephyr/kernel.h>
#include "test_global.h"

void test_large_packet_twt(struct k_sem *sem, void * test_settings);


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
