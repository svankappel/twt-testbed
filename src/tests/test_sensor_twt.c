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

#define STACK_SIZE 4096
#define PRIORITY 7   


#define MAX_THREADS 2  // Define the maximum number of threads

// Define the stack size and thread data structure
K_THREAD_STACK_ARRAY_DEFINE(thread_stacks, MAX_THREADS, STACK_SIZE);
struct k_thread thread_data[MAX_THREADS];

bool test_running = false;

struct test_sensor_twt_settings test_settings;

//K_SEM_DEFINE(end_sem, 0, 1);

//--------------------------------------------------------------------     
// Callback function to handle TWT events
//--------------------------------------------------------------------
void handle_twt_event()
{/*
    char buf[32];
    if(test_running)
    {
        sprintf(buf, "{\"sensor-value\":%d}", iter++);
        coap_put("test/test2",buf,6000);
    }
    if(iter >= test_settings.iterations)
    {
        k_sem_give(&end_sem);
    }*/
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
void configure_twt()
{
    //wifi_twt_setup(test_settings.twt_wake_interval, test_settings.twt_interval);
}

//--------------------------------------------------------------------
// Function to run the test
//--------------------------------------------------------------------
void run_test(int * iter)
{
    char buf[32];
    while(true)
    {
        sprintf(buf, "{\"sensor-value\":%d}", (*iter)++);
        coap_put("test/test1",buf,1000);
        sprintf(buf, "{\"sensor-value\":%d}", (*iter)++);
        coap_put("test/test1",buf,1000);

        k_sleep(K_SECONDS(5));

        if((*iter) >= test_settings.iterations)
        {
            break;
        }
    }
    //k_sem_take(&end_sem, K_FOREVER);
}
//--------------------------------------------------------------------


// Thread function that runs the test
void thread_function(void *arg1, void *arg2, void *arg3) 
{
    int iter=0;
    // Extract the semaphore and test settings
    struct k_sem *start_sem = (struct k_sem *)arg1;
    memcpy(&test_settings, arg2, sizeof(test_settings));

    // Wait for the semaphore to start the test
    k_sem_take(start_sem, K_FOREVER);

    LOG_INF("Starting test %d setup", test_settings.test_number);

    // configure power save mode 
    configure_ps();
    LOG_DBG("Power save mode configured");
    int ret;
    // connect to wifi
    ret = wifi_connect();
    k_sleep(K_SECONDS(1));
    if(ret != 0)
    {
        LOG_ERR("Failed to connect to wifi");
        k_sleep(K_FOREVER);
    }
    LOG_DBG("Connected to wifi");
    
    // configure TWT
    wifi_twt_register_event_callback(handle_twt_event,100);
    configure_twt(&test_settings);
    LOG_DBG("TWT configured");

    // run the test
    LOG_INF("Starting test sensor TWT %d", test_settings.test_number);
    profiler_output_binary(test_settings.test_number);
    test_running = true;
    run_test(&iter);
    test_running = false;
    profiler_all_clear();
    LOG_INF("Test sensor TWT %d finished", test_settings.test_number);

    // tear down TWT and disconnect from wifi
    if(wifi_twt_is_enabled())
    {
        wifi_twt_teardown();
    }

    k_sleep(K_SECONDS(1));

    ret = wifi_disconnect();
    if(ret != 0)
    {
        LOG_ERR("Failed to disconnect from wifi");
        k_sleep(K_FOREVER);
    }

    // give the semaphore to start the next test
    k_sem_give(start_sem);
}

// Function to initialize the test
void init_test_sensor_twt(struct k_sem *sem, void * test_settings) {
    
    
    static int testnb = 0;
    if (testnb > MAX_THREADS) {
        LOG_ERR("Max number of threads reached for this test");
        return;
    }
    k_tid_t thread_id = k_thread_create(&thread_data[testnb], thread_stacks[testnb],
                                        K_THREAD_STACK_SIZEOF(thread_stacks[testnb]),
                                        thread_function,
                                        sem, test_settings, NULL,
                                        PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(thread_id, "test_thread");
    k_thread_start(thread_id);
    testnb++;
}
