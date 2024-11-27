#ifndef TEST_ACTUATOR_TWT_H
#define TEST_ACTUATOR_TWT_H

#include <zephyr/kernel.h>


void test_actuator_twt(struct k_sem *sem, void * test_settings);

struct test_actuator_twt_settings {
    uint32_t twt_interval;
    uint32_t twt_wake_interval;
    uint8_t test_id;
    uint32_t iterations;
    uint32_t test_time_s;
    uint32_t wake_ahead_ms;
};

#endif // TEST_ACTUATOR_TWT_H
