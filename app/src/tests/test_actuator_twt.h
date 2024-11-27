#ifndef TEST_ACTUATOR_TWT_H
#define TEST_ACTUATOR_TWT_H

#include <zephyr/kernel.h>
#include "test_global.h"

void test_actuator_twt(struct k_sem *sem, void * test_settings);

struct test_actuator_twt_settings {
    uint32_t twt_interval;
    uint32_t twt_wake_interval;
    uint8_t test_id;
    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
    uint32_t min_interval;
    uint32_t max_interval;
    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
    uint32_t test_time_s;
};

#endif // TEST_ACTUATOR_TWT_H
