#include "test_template.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include "../wifi/wifi_sta.h"
#include "../coap.h"
#include "../profiler.h"
LOG_MODULE_REGISTER(test_template, CONFIG_MY_MAIN_LOG_LEVEL);

#define STACK_SIZE 4096
#define PRIORITY -7    // high proirity to ensure the thread is not preempted until blocked by the semaphore
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
    
}

//--------------------------------------------------------------------
// Function to configure TWT (Target Wake Time)
//--------------------------------------------------------------------
void configure_twt(struct test_template_settings *test_settings)
{
    wifi_twt_setup(test_settings->twt_interval, test_settings->twt_wake_interval);
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
    struct test_template_settings test_settings;
    memcpy(&test_settings, arg2, sizeof(test_settings));

    k_sem_take(sem, K_FOREVER);

    configure_ps();
    wifi_connect();
    k_sleep(K_SECONDS(1));
    wifi_twt_register_event_callback(handle_twt_event);
    configure_twt(&test_settings);

    LOG_INF("Starting test template %d", test_settings.test_number);
    profiler_output_binary(test_settings.test_number);
    run_test();
    profiler_all_clear();
    LOG_INF("Test %d finished", test_settings.test_number);

    if(wifi_twt_is_enabled())
    {
        wifi_twt_teardown();
    }

    k_sleep(K_SECONDS(1));

    wifi_disconnect();

    k_sem_give(sem);
}

// Function to initialize the test
void init_test_template(struct k_sem *sem, void * test_settings) {
    static int i = 0;
    if (i > MAX_THREADS) {
        LOG_ERR("Max number of threads reached for this test");
        return;
    }
    k_tid_t thread_id = k_thread_create(&thread_data[i], thread_stacks[i],
                                        K_THREAD_STACK_SIZEOF(thread_stacks[i]),
                                        thread_function,
                                        sem, test_settings, NULL,
                                        PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(thread_id, "test_thread");
    k_thread_start(thread_id);
    i++;
}
