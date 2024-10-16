#include "test_template.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include "../wifi/wifi_sta.h"
#include "../coap.h"
#include "../profiler.h"
LOG_MODULE_REGISTER(test_template, CONFIG_MY_MAIN_LOG_LEVEL);

#define STACK_SIZE 1024
#define PRIORITY 7

K_THREAD_STACK_DEFINE(thread_stack, STACK_SIZE);
struct k_thread thread_data;

void thread_function(void *arg1, void *arg2, void *arg3) 
{
    struct k_sem *sem = (struct k_sem *)arg1;
    k_sem_take(sem, K_FOREVER);

    LOG_INF("Starting test");
    k_sleep(K_SECONDS(10));
    LOG_INF("Test finished");

    k_sem_give(sem);
}

void init_test_template(struct k_sem *sem) {
    k_tid_t thread_id = k_thread_create(&thread_data, thread_stack,
                                        K_THREAD_STACK_SIZEOF(thread_stack),
                                        thread_function,
                                        sem, NULL, NULL,
                                        PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(thread_id, "test_thread");
    k_thread_start(thread_id);
}

