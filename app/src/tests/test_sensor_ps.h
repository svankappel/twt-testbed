#ifndef TEST_SENSOR_PS_H
#define TEST_SENSOR_PS_H

#include <zephyr/kernel.h>
#include "test_global.h"

void test_sensor_ps(struct k_sem *sem, void * test_settings);

struct test_sensor_ps_settings {
    uint32_t send_interval;
    uint8_t test_id;
    uint32_t iterations;
    uint8_t ps_mode;
    uint8_t ps_wakeup_mode;
};

#endif // TEST_SENSOR_PS_H

