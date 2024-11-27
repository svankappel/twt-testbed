#ifndef CONFIG_COAP_TWT_TESTBED_SERVER

#include "test_actuator_ps.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include "wifi_sta.h"
#include "wifi_ps.h"
#include "coap.h"

#ifdef CONFIG_PROFILER_ENABLE
#include "profiler.h"
#endif //CONFIG_PROFILER_ENABLE

LOG_MODULE_REGISTER(test_actuator_ps, CONFIG_MY_TEST_LOG_LEVEL);

#define STACK_SIZE 8192
#define PRIORITY -2         //non preemptive priority
static K_THREAD_STACK_DEFINE(thread_stack, STACK_SIZE);

static struct test_actuator_ps_settings test_settings;

static K_SEM_DEFINE(end_sem, 0, 1);

static bool test_failed = false;

struct test_control{
    int received;
};

static struct test_control control = { 0 };

static void print_test_results() {

    // Print the results
    LOG_INF("\n\n"
            "================================================================================\n"
            "=                          TEST RESULTS - ACTUATOR PS                          =\n"
            "================================================================================\n"
            "=  Test setup                                                                  =\n"
            "================================================================================\n"
            "=  Test Number:                           %6d                               =\n"
            "-------------------------------------------------------------------------------=\n"
            "=  PS Mode:                               %s                               =\n"
            "=  PS Wake-Up mode:              %s                               =\n"
            "=  Listen Interval:                       %6d                               =\n"
            "================================================================================\n"
            "=  Stats                                                                       =\n"
            "================================================================================\n"
            "=  Test time:                             %6d s                             =\n"
            "-------------------------------------------------------------------------------=\n"
            "=  Responses received:                    %6d                               =\n"
            "================================================================================\n",
            test_settings.test_id,
            test_settings.ps_mode ? "   WMM" : "Legacy",
            test_settings.ps_wakeup_mode ? "Listen Interval" : "           DTIM",
            CONFIG_PS_LISTEN_INTERVAL,
            test_settings.test_time_s,
            control.received);
}


//--------------------------------------------------------------------     
// Callback function to be called if wifi is disconnected unexpectedly
//--------------------------------------------------------------------
static void wifi_disconnected_event()
{
    LOG_ERR("Disconnected from wifi unexpectedly. Stopping test ...");
    test_failed = true;
    k_sem_give(&end_sem);
}

//--------------------------------------------------------------------     
// Callback function to handle coap responses
//--------------------------------------------------------------------
static void handle_coap_response(uint32_t time)
{
    if(test_failed){
        return;
    }

    control.received++;
}

//--------------------------------------------------------------------
// Function to configure power save mode
//--------------------------------------------------------------------
static void configure_ps()
{
    if(test_settings.ps_mode == PS_MODE_LEGACY){
        wifi_ps_mode_legacy();
    }else{
        wifi_ps_mode_wmm();
    }

    if(test_settings.ps_wakeup_mode == PS_WAKEUP_MODE_DTIM){
        wifi_ps_wakeup_dtim();
    }else{
        wifi_ps_wakeup_listen_interval();
    }
}

//--------------------------------------------------------------------
// Function to run the test
//--------------------------------------------------------------------
static void run_test()
{
    
    k_sem_take(&end_sem, K_SECONDS(test_settings.test_time_s));
    
}

//--------------------------------------------------------------------
// Thread function that runs the test
static void thread_function(void *arg1, void *arg2, void *arg3) 
{
    memset(&control, 0, sizeof(control));

    // Extract the semaphore and test settings
    struct k_sem *test_sem = (struct k_sem *)arg1;
    memcpy(&test_settings, arg2, sizeof(test_settings));

    LOG_INF("Starting test %d setup", test_settings.test_id);

    int ret;

    // wifi
    configure_ps();
    wifi_register_disconnected_cb(wifi_disconnected_event);
    ret = wifi_connect();
    if(ret != 0){
        LOG_ERR("Failed to connect to wifi");
        k_sleep(K_FOREVER);
    }
    LOG_DBG("Connected to wifi");
    k_sleep(K_SECONDS(5));

    //coap
    coap_observe(CONFIG_COAP_ACTUATOR_TEST_RESOURCE, NULL);
    coap_register_response_callback(handle_coap_response);
    k_sleep(K_SECONDS(1));
    

    // run the test
    LOG_INF("Starting test %d", test_settings.test_id);
    #ifdef CONFIG_PROFILER_ENABLE
    profiler_output_binary(test_settings.test_id);
    #endif //CONFIG_PROFILER_ENABLE
    memset(&control, 0, sizeof(control));
    run_test();
    #ifdef CONFIG_PROFILER_ENABLE
    profiler_all_clear();
    #endif //CONFIG_PROFILER_ENABLE

    //finish test
    if(!test_failed){
        LOG_INF("Test %d finished", test_settings.test_id);

        coap_register_response_callback(NULL);
        coap_cancel_observers();

        k_sleep(K_SECONDS(2));

        ret = wifi_disconnect();
        if(ret != 0)
        {
            LOG_ERR("Failed to disconnect from wifi");
            k_sleep(K_FOREVER);
        }
    }
    else{ //test failed
        LOG_ERR("Test %d failed", test_settings.test_id);

        //coap
        coap_register_response_callback(NULL);
        coap_cancel_observers();
    }

    print_test_results();

    k_sleep(K_SECONDS(2)); 

    // give the semaphore to start the next test
    k_sem_give(test_sem);
}

// Function to initialize the test
void test_actuator_ps(struct k_sem *sem, void * test_settings) {
    
    struct k_thread thread_data;

    k_tid_t thread_id = k_thread_create(&thread_data, thread_stack,
                                        K_THREAD_STACK_SIZEOF(thread_stack),
                                        thread_function,
                                        sem, test_settings, NULL,
                                        PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(thread_id, "test_thread");
    k_thread_start(thread_id);

    //wait for the test to finish
    k_sem_take(sem, K_FOREVER);

    //make sure the thread is stopped
    k_thread_abort(thread_id);  
}


#endif // CONFIG_COAP_TWT_TESTBED_SERVER