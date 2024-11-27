#ifndef CONFIG_COAP_TWT_TESTBED_SERVER

#include "test_sensor_ps.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include "wifi_sta.h"
#include "wifi_ps.h"
#include "coap.h"

#ifdef CONFIG_PROFILER_ENABLE
#include "profiler.h"
#endif //CONFIG_PROFILER_ENABLE

LOG_MODULE_REGISTER(test_sensor_ps, CONFIG_MY_TEST_LOG_LEVEL);

#define STACK_SIZE 8192
#define PRIORITY -2         //non preemptive priority
static K_THREAD_STACK_DEFINE(thread_stack, STACK_SIZE);

static void handle_timer_event();
static K_TIMER_DEFINE(send_timer, handle_timer_event, NULL);

static struct test_sensor_ps_settings test_settings;

static K_SEM_DEFINE(end_sem, 0, 1);
static K_SEM_DEFINE(timer_event_sem, 0, 1);

static bool test_failed = false;

struct test_control{
    int iter;
    int sent;
    int received;
    uint32_t latency_sum;
};

static struct test_control control = { 0 };

static void print_test_results() {
    // Check for inconsistencies and print warnings
    if ((control.iter != test_settings.iterations)) {
        LOG_WRN("Warning: Test could not complete all iterations");
    }

    // Print the results
    LOG_INF("\n\n"
            "================================================================================\n"
            "=                           TEST RESULTS - SENSOR PS                           =\n"
            "================================================================================\n"
            "=  Test setup                                                                  =\n"
            "================================================================================\n"
            "=  Test Number:                           %6d                               =\n"
            "=  Iterations:                            %6d                               =\n"
            "-------------------------------------------------------------------------------=\n"
            "=  PS Mode:                               %s                               =\n"
            "=  PS Wake-Up mode:              %s                               =\n"
            "=  Listen Interval:                       %6d                               =\n"
            "================================================================================\n"
            "=  Stats                                                                       =\n"
            "================================================================================\n"
            "=  Requests sent:                         %6d                               =\n"
            "-------------------------------------------------------------------------------=\n"
            "=  Responses received:                    %6d                               =\n"
            "=------------------------------------------------------------------------------=\n"
            "=  Average latency:                       %6d ms                            =\n"
            "================================================================================\n",
            test_settings.test_id,
            control.iter,
            test_settings.ps_mode ? "   WMM" : "Legacy",
            test_settings.ps_wakeup_mode ? "Listen Interval" : "           DTIM",
            CONFIG_PS_LISTEN_INTERVAL,
            control.sent,
            control.received,
            control.received == 0 ? -1 : control.latency_sum/control.received);
}


//--------------------------------------------------------------------     
// Callback function to handle timer event
//--------------------------------------------------------------------
static void handle_timer_event()
{
    k_timer_start(&send_timer, K_MSEC(test_settings.send_interval), K_NO_WAIT);
    k_sem_give(&timer_event_sem);
}

//--------------------------------------------------------------------     
// Callback function to be called if wifi is disconnected unexpectedly
//--------------------------------------------------------------------
static void wifi_disconnected_event()
{
    LOG_ERR("Disconnected from wifi unexpectedly. Stopping test ...");
    test_failed = true;
    k_sem_give(&timer_event_sem);
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
    control.latency_sum += time;

    if((control.iter >= test_settings.iterations))
    {
        k_sem_give(&end_sem);
    }
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
    k_timer_start(&send_timer, K_MSEC(test_settings.send_interval), K_NO_WAIT);

    while(true){
        k_sem_take(&timer_event_sem, K_FOREVER);

        if(test_failed){
            break;
        }

        char buf[32];
        int ret;

        if(control.iter < test_settings.iterations){
            sprintf(buf, "{\"sensor-value\":%d}", control.iter++);
            ret = coap_put(CONFIG_COAP_SENSOR_LARGE_PACKET_TEST_RESOURCE, buf);
            if(ret >= 0){
            control.sent++;
        } 
        }else{
            break;
        }
    }
    k_sem_take(&end_sem, K_MSEC(test_settings.send_interval));
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

    //register coap response callback
    coap_register_response_callback(handle_coap_response);

    // configure power save mode 
    configure_ps();
    LOG_DBG("Power save mode configured");
    int ret;
    // connect to wifi
    ret = wifi_connect();
    if(ret != 0){
        LOG_ERR("Failed to connect to wifi");
        k_sleep(K_FOREVER);
    }
    LOG_DBG("Connected to wifi");

    k_sleep(K_SECONDS(1));

    wifi_register_disconnected_cb(wifi_disconnected_event);

    k_sleep(K_SECONDS(5));

    coap_init_pool(1000);  // init coap request pool with 1s request timeout

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

    coap_register_response_callback(NULL);

    if(!test_failed){
        LOG_INF("Test %d finished", test_settings.test_id);

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
    }

    print_test_results();

    k_sleep(K_SECONDS(2)); //give time for the logs to print

    // give the semaphore to start the next test
    k_sem_give(test_sem);
}

// Function to initialize the test
void test_sensor_ps(struct k_sem *sem, void * test_settings) {
    
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