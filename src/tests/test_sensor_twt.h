#ifndef TEST_SENSOR_TWT_H
#define TEST_SENSOR_TWT_H

#include <zephyr/kernel.h>


void test_sensor_twt(struct k_sem *sem, void * test_settings);

struct test_sensor_twt_settings {
    uint32_t twt_interval;
    uint32_t twt_wake_interval;
    uint8_t test_number;
    uint32_t iterations;
    uint32_t wake_ahead_ms;
};

#endif // TEST_SENSOR_TWT_H
