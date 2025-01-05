#ifndef TEST_LARGE_PACKET_PS_H
#define TEST_LARGE_PACKET_PS_H

#include <zephyr/kernel.h>
#include "test_global.h"


void test_large_packet_ps(void * test_settings);


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

