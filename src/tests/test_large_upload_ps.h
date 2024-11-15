#ifndef TEST_LARGE_UPLOAD_PS_H
#define TEST_LARGE_UPLOAD_PS_H

#include <zephyr/kernel.h>

#define PS_MODE_DISABLED 0
#define PS_MODE_ENABLED 1

#define PS_MODE_LEGACY 0
#define PS_MODE_WMM 1

#define PS_WAKEUP_MODE_DTIM 0
#define PS_WAKEUP_MODE_LISTEN_INTERVAL 1

void test_large_upload_ps(struct k_sem *sem, void * test_settings);

struct test_large_upload_ps_settings {
    uint32_t send_interval;
    uint8_t test_id;
    uint32_t bytes;
    uint32_t iterations;
    uint8_t ps_enabled;
    uint8_t ps_mode;
    uint8_t ps_wakeup_mode;
};

#endif // TEST_LARGE_UPLOAD_PS_H

