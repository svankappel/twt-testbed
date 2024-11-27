#ifndef TEST_ACTUATOR_PS_H
#define TEST_ACTUATOR_PS_H

#include <zephyr/kernel.h>

#define PS_MODE_DISABLED 0
#define PS_MODE_ENABLED 1

#define PS_MODE_LEGACY 0
#define PS_MODE_WMM 1

#define PS_WAKEUP_MODE_DTIM 0
#define PS_WAKEUP_MODE_LISTEN_INTERVAL 1

void test_actuator_ps(struct k_sem *sem, void * test_settings);

struct test_actuator_ps_settings {
    uint8_t test_id;
    uint32_t test_time_s;
    uint32_t iterations;
    uint8_t ps_mode;
    uint8_t ps_wakeup_mode;
};

#endif // TEST_ACTUATOR_PS_H

