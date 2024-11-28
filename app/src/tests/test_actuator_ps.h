#ifndef TEST_ACTUATOR_PS_H
#define TEST_ACTUATOR_PS_H

#include <zephyr/kernel.h>
#include "test_global.h"


void test_actuator_ps(struct k_sem *sem, void * test_settings);

struct test_actuator_ps_settings {
    uint8_t test_id;
    uint32_t test_time_s;
    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
    uint32_t min_interval;
    uint32_t max_interval;
    bool echo;
    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
    uint8_t ps_mode;
    uint8_t ps_wakeup_mode;
};

#endif // TEST_ACTUATOR_PS_H

