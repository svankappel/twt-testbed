#ifndef TEST_TEMPLATE_H
#define TEST_TEMPLATE_H

#include <zephyr/kernel.h>

void init_test_template(struct k_sem *sem, void * test_settings);

struct test_template_settings {
    uint32_t twt_interval;
    uint32_t twt_wake_interval;
    uint8_t test_number;
};


#endif // TEST_TEMPLATE_H