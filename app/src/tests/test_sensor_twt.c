#ifdef CONFIG_COAP_TWT_TESTBED_SERVER

#include "test_sensor_twt.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include "wifi_sta.h"
#include "wifi_ps.h"
#include "wifi_twt.h"
#include "coap.h"
#include "profiler.h"
LOG_MODULE_REGISTER(test_sensor_twt, CONFIG_MY_TEST_LOG_LEVEL);

#define STACK_SIZE 8192
#define PRIORITY -2         //non preemptive priority
static K_THREAD_STACK_DEFINE(thread_stack, STACK_SIZE);

#define MAX_INTERVALS_BUFFERED 50


static struct test_sensor_twt_settings test_settings;

static K_SEM_DEFINE(end_sem, 0, 1);
static K_SEM_DEFINE(wake_ahead_sem, 0, 1);

static bool test_failed = false;

struct test_control{
    int iter;
    int sent;
    int received;
    int received_serv;
    uint32_t latency_sum;
    uint16_t latency_hist[MAX_INTERVALS_BUFFERED];
};

static struct test_control control = { 0 };

static void print_test_results() {
    // Check for inconsistencies and print warnings
    if ((control.iter != test_settings.iterations)) {
        LOG_WRN("Warning: Test could not complete all iterations");
    }
    
    if (control.received_serv < 0) {
        LOG_WRN("Warning: Could not receive server stats");
    }


    // Print the results
    LOG_INF("\n\n"
            "================================================================================\n"
            "=                          TEST RESULTS - SENSOR TWT                           =\n"
            "================================================================================\n"
            "=  Test setup                                                                  =\n"
            "================================================================================\n"
            "=  Test Number:                           %6d                               =\n"
            "=  Iterations:                            %6d                               =\n"
            "=------------------------------------------------------------------------------=\n"
            "=  Negotiated TWT Interval:               %6d s                             =\n"
            "=  Negotiated TWT Wake Interval:          %6d ms                            =\n"
            "================================================================================\n"
            "=  Stats                                                                       =\n"
            "================================================================================\n"
            "=  Requests sent:                         %6d                               =\n"
            "-------------------------------------------------------------------------------=\n"
            "=  Requests received on server:           %6d                               =\n"
            "-------------------------------------------------------------------------------=\n"
            "=  Responses received:                    %6d                               =\n"
            "=------------------------------------------------------------------------------=\n"
            "=  Requests lost:                         %6d                               =\n"
            "=  Responses lost:                        %6d                               =\n"
            "=------------------------------------------------------------------------------=\n"
            "=  Average latency:                       %6d s                             =\n"
            "================================================================================\n",
            test_settings.test_id,
            control.iter,
            wifi_twt_get_interval_ms() / 1000,
            wifi_twt_get_wake_interval_ms(),
            control.sent,
            control.received_serv,
            control.received,
            control.received_serv < 0 ? -1 : control.sent - control.received_serv,
            control.received_serv < 0 ? -1 : control.received_serv - control.received,
            control.latency_sum/control.received);
        

    // Print the latency histogram
    char hist_str[1024] = {0};
    char temp[32];
    for (int i = 0; i < MAX_INTERVALS_BUFFERED; i++) {
        
        snprintf(temp, sizeof(temp), "%d;%d\n", i * test_settings.twt_interval / 1000, control.latency_hist[i]);
        strncat(hist_str, temp, sizeof(hist_str) - strlen(hist_str) - 1);
        
    }
    snprintf(temp, sizeof(temp), "lost;%d\n", control.sent - control.received);
    strncat(hist_str, temp, sizeof(hist_str) - strlen(hist_str) - 1);
    LOG_INF("Latency Histogram:\n%s", hist_str);

}


//--------------------------------------------------------------------     
// Callback function to handle TWT session wake ahead event
//--------------------------------------------------------------------
static void handle_twt_event()
{
    k_sem_give(&wake_ahead_sem);
}

//--------------------------------------------------------------------     
// Callback function to be called if wifi is disconnected unexpectedly
//--------------------------------------------------------------------
static void wifi_disconnected_event()
{
    LOG_ERR("Disconnected from wifi unexpectedly. Stopping test ...");

    //test to ignore disconnection
    
    test_failed = true;
    k_sem_give(&wake_ahead_sem);
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

    control.latency_sum += time/1000;

    int index = (time+(test_settings.twt_interval/2)) / test_settings.twt_interval;
    if (index < MAX_INTERVALS_BUFFERED) {
        control.latency_hist[index]++;
    }
    

    if((control.iter>=test_settings.iterations) && (control.received == control.sent))
    {
        k_sem_give(&end_sem);
    }
}


//--------------------------------------------------------------------
// Function to configure TWT (Target Wake Time)
//--------------------------------------------------------------------
static void configure_twt()
{
    wifi_twt_setup(test_settings.twt_wake_interval, test_settings.twt_interval);
}

//--------------------------------------------------------------------
// Function to run the test
//--------------------------------------------------------------------
static void run_test()
{
    while(true){
        k_sem_take(&wake_ahead_sem, K_FOREVER);

        if(test_failed){
            break;
        }

        char buf[32];
        int ret;

        if(control.iter < test_settings.iterations){
            sprintf(buf, "{\"sensor-value\":%d}", control.iter++);
            ret = coap_put("sensor", buf);
            if(ret >= 0){
                control.sent++;
            } 
        }else{
            break;
        }
    }
    k_sem_take(&end_sem, K_MSEC(test_settings.twt_interval*2));
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

    ret = coap_validate();
    if(ret != 0){
        LOG_ERR("Failed to validate CoAP client");
        k_sleep(K_FOREVER);
    }
    LOG_DBG("CoAP validated");

    k_sleep(K_SECONDS(5));

    coap_init_pool(test_settings.twt_interval * MAX_INTERVALS_BUFFERED);  // init coap request pool with 1s request timeout


    // configure TWT
    wifi_twt_register_event_callback(handle_twt_event,test_settings.wake_ahead_ms);
    configure_twt(&test_settings);
    LOG_DBG("TWT configured");

    // run the test
    LOG_INF("Starting test %d", test_settings.test_id);
    profiler_output_binary(test_settings.test_id);

    run_test();

    profiler_all_clear();

    coap_register_response_callback(NULL);

    if(!test_failed){
        LOG_INF("Test %d finished", test_settings.test_id);

        // tear down TWT and disconnect from wifi
        if(wifi_twt_is_enabled())
        {
            wifi_twt_teardown();
        }

        k_sleep(K_SECONDS(2));

        control.received_serv = coap_get_stat();

        ret = wifi_disconnect();
        if(ret != 0)
        {
            LOG_ERR("Failed to disconnect from wifi");
            k_sleep(K_FOREVER);
        }
    }
    else{ //test failed
        LOG_ERR("Test %d failed", test_settings.test_id);
        control.received_serv = -1;
    }

    print_test_results();

    k_sleep(K_SECONDS(2)); //give time for the logs to print

    // give the semaphore to start the next test
    k_sem_give(test_sem);
}

// Function to initialize the test
void test_sensor_twt(struct k_sem *sem, void * test_settings) {
    
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