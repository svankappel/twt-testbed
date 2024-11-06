#ifndef TEST_SENSOR_PS_H
#define TEST_SENSOR_PS_H

#include <zephyr/kernel.h>


void test_sensor_ps(struct k_sem *sem, void * test_settings);

struct test_sensor_ps_settings {
    uint32_t send_interval;
    uint8_t test_number;
    uint32_t iterations;
};

#endif // TEST_SENSOR_PS_H
