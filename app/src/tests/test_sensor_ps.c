#ifdef CONFIG_COAP_TWT_TESTBED_SERVER

#include "test_sensor_ps.h"
#include "test_global.h"
#include "test_report.h"

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

static void handle_timer_event();
static K_TIMER_DEFINE(send_timer, handle_timer_event, NULL);

static struct test_sensor_ps_settings test_settings;

static K_SEM_DEFINE(timer_event_sem, 0, 1);

struct test_control{
    bool test_failed;
};
static struct test_control control = { 0 };


struct test_monitor{
    int iter;
    int sent;
    int received;
    int received_serv;
    uint32_t latency_sum;
};

static struct test_monitor monitor = { 0 };

static void print_test_results() {
    // Check for inconsistencies and print warnings
    if ((monitor.iter != test_settings.iterations)) {
        LOG_WRN("Warning: Test could not complete all iterations");
    }
    
    if (monitor.received_serv < 0) {
        LOG_WRN("Warning: Could not receive server stats");
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
            "=  Requests received on server:           %6d                               =\n"
            "-------------------------------------------------------------------------------=\n"
            "=  Responses received:                    %6d                               =\n"
            "=------------------------------------------------------------------------------=\n"
            "=  Requests lost:                         %6d                               =\n"
            "=  Responses lost:                        %6d                               =\n"
            "=------------------------------------------------------------------------------=\n"
            "=  Average latency:                       %6d ms                            =\n"
            "================================================================================\n",
            test_settings.test_id,
            monitor.iter,
            test_settings.ps_mode ? "   WMM" : "Legacy",
            test_settings.ps_wakeup_mode ? "Listen Interval" : "           DTIM",
            CONFIG_PS_LISTEN_INTERVAL,
            monitor.sent,
            monitor.received_serv,
            monitor.received,
            monitor.received_serv < 0 ? -1 : monitor.sent - monitor.received_serv,
            monitor.received_serv < 0 ? -1 : monitor.received_serv - monitor.received,
            monitor.received == 0 ? -1 : monitor.latency_sum/monitor.received);
}


static void generate_test_report(){
    struct test_report report;
    sprintf(report.test_title, "\"test_title\":\"Sensor Use Case - PS\"");

    sprintf(report.test_setup,
        "\"test_setup\":\n"
        "{\n"
            "\"Iterations\": %d,\n"
            "\"PS_Mode\": \"%s\",\n"
            "\"PS_Wake_Up_Mode\": \"%s\"\n"
        "}",
        test_settings.iterations,
        test_settings.ps_mode ? "WMM" : "Legacy",
        test_settings.ps_wakeup_mode ? "Listen Interval" : "DTIM");

    sprintf(report.results, 
        "\"results\":\n"
        "{\n"
            "\"Requests_Sent\": %d,\n"
            "\"Requests_Received_on_Server\": %d,\n"
            "\"Responses_Received\": %d,\n"
            "\"Requests_Lost\": %d,\n"
            "\"Responses_Lost\": %d,\n"
            "\"Average_Latency\": \"%d ms\"\n"
        "}",
        monitor.sent,
        monitor.received_serv,
        monitor.received,
        monitor.received_serv < 0 ? -1 : monitor.sent - monitor.received_serv,
        monitor.received_serv < 0 ? -1 : monitor.received_serv - monitor.received,
        monitor.received == 0 ? -1 : monitor.latency_sum / monitor.received);

    test_report_print(&report);
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
    control.test_failed = true;
    k_sem_give(&timer_event_sem);
}

//--------------------------------------------------------------------     
// Callback function to handle coap responses
//--------------------------------------------------------------------
static void handle_coap_response(uint32_t time, uint8_t * payload, uint16_t payload_len)
{
    if(control.test_failed){
        return;
    }

    monitor.received++;
    monitor.latency_sum += time;
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

        if(control.test_failed){
            break;
        }

        char buf[32];
        int ret;

        if(monitor.iter < test_settings.iterations){
            sprintf(buf, "{\"sensor-value\":%d}", monitor.iter++);
            ret = coap_put(TESTBED_SENSOR_RESOURCE, buf);
            if(ret >= 0){
                monitor.sent++;
            } 
        }else{
            break;
        }
    }
    k_sleep(K_MSEC(test_settings.send_interval));
}

//--------------------------------------------------------------------
// Thread function that runs the test
static void thread_function(void *arg1, void *arg2, void *arg3) 
{
    memset(&monitor, 0, sizeof(monitor));
    memset(&control, 0, sizeof(control));

    // Extract the semaphore and test settings
    struct k_sem *test_sem = (struct k_sem *)arg1;
    memcpy(&test_settings, arg2, sizeof(test_settings));

    LOG_INF("Starting test %d setup", test_settings.test_id);

    int ret;

    //wifi
    configure_ps();
    ret = wifi_connect();
    if(ret != 0){
        LOG_ERR("Failed to connect to wifi");
        k_sleep(K_FOREVER);
    }
    wifi_register_disconnected_cb(wifi_disconnected_event);
    k_sleep(K_SECONDS(5));


    //coap
    coap_register_put_response_callback(handle_coap_response);
    ret = coap_validate();
    if(ret != 0){
        LOG_ERR("Failed to validate CoAP client");
        k_sleep(K_FOREVER);
    }
    coap_init_pool(test_settings.send_interval); 
    k_sleep(K_SECONDS(2));


    // run the test
    LOG_INF("Starting test %d", test_settings.test_id);
    #ifdef CONFIG_PROFILER_ENABLE
    profiler_output_binary(test_settings.test_id);
    #endif //CONFIG_PROFILER_ENABLE
    memset(&monitor, 0, sizeof(monitor));
    run_test();
    #ifdef CONFIG_PROFILER_ENABLE
    profiler_all_clear();
    #endif //CONFIG_PROFILER_ENABLE


    //finish test
    if(!control.test_failed){
        LOG_INF("Test %d finished", test_settings.test_id);

        //coap
        coap_register_put_response_callback(NULL);
        monitor.received_serv = coap_get_stat();
        k_sleep(K_SECONDS(2));

        //wifi
        ret = wifi_disconnect();
        if(ret != 0){
            LOG_ERR("Failed to disconnect from wifi");
            k_sleep(K_FOREVER);
        }
    }
    else{ //test failed
        LOG_ERR("Test %d failed", test_settings.test_id);
    
        coap_register_put_response_callback(NULL);
        monitor.received_serv = -1;
    }

    print_test_results();

    generate_test_report();

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
                                        TEST_THREAD_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(thread_id, "test_thread");
    k_thread_start(thread_id);


    //wait for the test to finish
    k_sem_take(sem, K_FOREVER);

    //make sure the thread is stopped
    k_thread_abort(thread_id);  
}

#endif //CONFIG_COAP_TWT_TESTBED_SERVER