#include "test_sensor_twt.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include "../wifi/wifi_sta.h"
#include "../wifi/wifi_ps.h"
#include "../wifi/wifi_twt.h"
#include "../coap.h"
#include "../profiler.h"
LOG_MODULE_REGISTER(test_sensor_twt, CONFIG_MY_TEST_LOG_LEVEL);

K_THREAD_STACK_DEFINE(test_thread_stack, 4096);

int d;
struct k_thread debug_thread_data;



void coap_debug_thread(void)
{

    wifi_connect();

    d = 0;
    char buf[32];
    snprintf(buf, sizeof(buf), "payload");

    while (true)
    {
        snprintf(buf, sizeof(buf), "test-payload-%d", d++);
        coap_put("test/test1", buf, 1000);
        snprintf(buf, sizeof(buf), "test-payload-%d", d++);
        coap_put("test/test1", buf, 1000);

        k_sleep(K_SECONDS(5));
    }
}

void init_thread()
{
    k_tid_t test_thread_id = k_thread_create(&debug_thread_data, test_thread_stack ,4096, coap_debug_thread, NULL, NULL, NULL, 7, 0, K_NO_WAIT);
    k_thread_name_set(test_thread_id, "test_thread");
    k_thread_start(test_thread_id);
}