#include "test_template.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include "../wifi/wifi_sta.h"
#include "../coap.h"
#include "../profiler.h"
LOG_MODULE_REGISTER(test_template, CONFIG_MY_MAIN_LOG_LEVEL);

#define STACK_SIZE 4096
#define PRIORITY 7
#define MAX_THREADS 2  // Define the maximum number of threads

// Define the stack size and thread data structure
K_THREAD_STACK_ARRAY_DEFINE(thread_stacks, MAX_THREADS, STACK_SIZE);
struct k_thread thread_data[MAX_THREADS];

//--------------------------------------------------------------------
// Callback function to handle TWT events
//--------------------------------------------------------------------
static void handle_twt_event(const int awake)
{
    
}

//--------------------------------------------------------------------
// Function to configure power save mode
//--------------------------------------------------------------------
void configure_ps()
{
    wifi_ps_enable();
}

//--------------------------------------------------------------------
// Function to configure TWT (Target Wake Time)
//--------------------------------------------------------------------
void configure_twt()
{
    //wifi_twt_setup(100, 1000);
}

//--------------------------------------------------------------------
// Function to run the test
//--------------------------------------------------------------------
void run_test()
{
    k_sleep(K_SECONDS(10));
}
//--------------------------------------------------------------------


// Thread function that runs the test
void thread_function(void *arg1, void *arg2, void *arg3) 
{
    struct k_sem *sem = (struct k_sem *)arg1;
    uint8_t profiler_output = *((uint8_t*)arg2);
    k_sem_take(sem, K_FOREVER);

    configure_ps();
    wifi_connect();
    k_sleep(K_SECONDS(1));
    wifi_twt_register_event_callback(handle_twt_event);
    configure_twt();

    LOG_INF("Starting test template");
    profiler_output_next();
    run_test();
    profiler_all_clear();
    LOG_INF("Test template finished");

    if(wifi_twt_is_enabled())
    {
        wifi_twt_teardown();
    }

    k_sleep(K_SECONDS(1));

    wifi_disconnect();

    k_sem_give(sem);
}

// Function to initialize the test
void init_test_template(struct k_sem *sem) {
    static int i = 0;
    if (i > MAX_THREADS) {
        LOG_ERR("Max number of threads reached for this test");
        return;
    }
    k_tid_t thread_id = k_thread_create(&thread_data[i], thread_stacks[i],
                                        K_THREAD_STACK_SIZEOF(thread_stacks[i]),
                                        thread_function,
                                        sem, NULL, NULL,
                                        PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(thread_id, "test_thread");
    k_thread_start(thread_id);
    i++;
}
