/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef CONFIG_COAP_TWT_TESTBED_SERVER

#include "test_multi_packet_twt.h"
#include "test_global.h"
#include "test_report.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include "wifi_sta.h"
#include "wifi_ps.h"
#include "wifi_twt.h"
#include "coap.h"

#ifdef CONFIG_PROFILER_ENABLE
#include "profiler.h"
#endif //CONFIG_PROFILER_ENABLE

LOG_MODULE_REGISTER(test_multi_packet_twt, CONFIG_MY_TEST_LOG_LEVEL);


#define MAX_INTERVALS_BUFFERED 50


static struct test_multi_packet_twt_settings test_settings;

static K_SEM_DEFINE(wake_ahead_sem, 0, 1);
static K_SEM_DEFINE(send_sem, 0, 1);

struct test_control{
    bool test_failed;
    int iter_sent;
    int iter_received;
    bool ready_to_send;
};
static struct test_control control = { 0 };

struct test_monitor{
    int iter;
    int sent;
    int received;
    uint32_t latency_sum;
    uint16_t latency_hist[MAX_INTERVALS_BUFFERED];
};

static struct test_monitor monitor = { 0 };

/**
 * @brief Generates a test report
 *
 * This function initializes a test_report structure, populates it with
 * test details including the test title, setup, and results, and then
 * prints the report using the test_report_print function.
 *
 * The report is formatted as a JSON string.
 */
static void generate_test_report(){
    struct test_report report;
    memset(&report, '\0', sizeof(report));

    sprintf(report.test_title, "\"test_title\":\"Multi Packet Use Case - TWT\"");

    sprintf(report.test_setup,
        "\"test_setup\":\n"
        "{\n"
            "\"Iterations\": %d,\n"
            "\"Number_of_packet_per_Iteration\": %d,\n"
            "\"Negotiated_TWT_Interval\": \"%d s\",\n"
            "\"Negotiated_TWT_Wake_Interval\": \"%d ms\"\n"
        "}",
        test_settings.iterations,
        test_settings.packet_number,
        wifi_twt_get_interval_ms() / 1000,
        wifi_twt_get_wake_interval_ms());

    sprintf(report.results, 
        "\"results\":\n"
        "{\n"
            "\"Requests_Sent\": %d,\n"
            "\"Responses_Received\": %d,\n"
            "\"Average_Latency\": \"%d s\"\n"
        "}",
        monitor.sent,
        monitor.received,
        monitor.received == 0 ? -1 : monitor.latency_sum / monitor.received);

    
    // Generate latency histogram
    char temp[64];
    sprintf(report.latency_histogram, "\"latency_histogram\":\n[\n");
    for (int i = 0; i < 20; i++) {
        sprintf(temp, "{ \"latency\": %d, \"count\": %d },\n", i * test_settings.twt_interval / 1000, monitor.latency_hist[i]);
        strncat(report.latency_histogram, temp, sizeof(report.latency_histogram) - strlen(report.latency_histogram) - 1);
    }
    sprintf(temp, "{ \"latency\": \"lost\", \"count\": %d }\n", monitor.sent - monitor.received);
    strncat(report.latency_histogram, temp, sizeof(report.latency_histogram) - strlen(report.latency_histogram) - 1);
    strncat(report.latency_histogram, "]", sizeof(report.latency_histogram) - strlen(report.latency_histogram) - 1);

    test_report_print(&report);
    
}

//--------------------------------------------------------------------
// Set PS to DTIM legacy (default PS mode)
//--------------------------------------------------------------------
static void configure_ps()
{
    wifi_ps_mode_legacy();
    wifi_ps_wakeup_dtim();
}


//--------------------------------------------------------------------     
// Callback function to handle TWT session wake ahead event
//--------------------------------------------------------------------
static void handle_twt_event()
{
    if(control.ready_to_send){
        k_sem_give(&wake_ahead_sem);
    }
}

//--------------------------------------------------------------------     
// Callback function to be called if wifi is disconnected unexpectedly
//--------------------------------------------------------------------
static void wifi_disconnected_event()
{
    LOG_ERR("Disconnected from wifi unexpectedly. Stopping test ...");
    
    control.test_failed = true;
    k_sem_give(&wake_ahead_sem); 
    k_sem_give(&send_sem);
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
    monitor.latency_sum += time/1000;

    //compute latency histogram
    int index = (time+(test_settings.twt_interval/2)) / test_settings.twt_interval;
    if (index < MAX_INTERVALS_BUFFERED) {
        monitor.latency_hist[index]++;
    }

    //check if all packets have been received and give the semaphore to start the next iteration
    control.iter_received++;
    if(control.iter_received == control.iter_sent && control.iter_sent == test_settings.packet_number){
        k_sem_give(&send_sem);
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
    k_sem_give(&send_sem);

    while(true){
        //wait for the last iteration to finish (timeout in case of packet lost)
        k_sem_take(&send_sem, K_MSEC(test_settings.twt_interval * test_settings.packet_number));
        
        k_sleep(K_SECONDS(2)); //wait min 2 sec between iterations

        //wait for wake ahead event
        control.ready_to_send = true;
        k_sem_take(&wake_ahead_sem, K_FOREVER);
        control.ready_to_send = false;

        coap_init_pool(0xFFFFFFFF); //clear the pool

        if(control.test_failed){
            break;
        }

        char buf[64];
        int ret;

        if(monitor.iter < test_settings.iterations){
            control.iter_sent = 0;
            control.iter_received = 0;
            for(int i = 0; i < test_settings.packet_number; i++){
                sprintf(buf, "{\"multi_packet-value\":%d, \"iter\":%d}",i, monitor.iter);
                ret = coap_put(CONFIG_COAP_SENSOR_TEST_RESOURCE, buf);
                if(ret >= 0){
                    monitor.sent++;
                    control.iter_sent++;
                } 
            }
            monitor.iter++;
        }else{
            break;
        }
    }
}
//--------------------------------------------------------------------


// Thread function that runs the test
static void thread_function(void *arg1, void *arg2, void *arg3) 
{
    memset(&monitor, 0, sizeof(monitor));
    memset(&control, 0, sizeof(control));

    // Extract the test settings
    memcpy(&test_settings, arg1, sizeof(test_settings));

    LOG_INF("Starting test %d setup", test_settings.test_id);

    int ret;

    // connect to wifi
    configure_ps();
    ret = wifi_connect();
    if(ret != 0){
        LOG_ERR("Failed to connect to wifi");
        k_sleep(K_FOREVER);
    }
    wifi_register_disconnected_cb(wifi_disconnected_event);
    k_sleep(K_SECONDS(5));

    // coap
    coap_register_put_response_callback(handle_coap_response);
    coap_init_pool(0xFFFFFFFF); //clear the pool
    coap_put(CONFIG_COAP_SENSOR_TEST_RESOURCE, "{init-message}"); //send a first message before activating TWT
    k_sleep(K_SECONDS(2));


    // configure TWT
    wifi_twt_register_event_callback(handle_twt_event,test_settings.wake_ahead_ms);
    configure_twt(&test_settings);


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

        // tear down TWT and disconnect from wifi
        if(wifi_twt_is_enabled()){
            wifi_twt_teardown();
        }
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
    }

    generate_test_report();

    k_sleep(K_SECONDS(2)); //give time for the logs to print

    // give the semaphore to start the next test
    k_sem_give(&test_sem);
}

// Function to initialize the test
void test_multi_packet_twt(void * test_settings) {
    
    struct k_thread thread_data;

    k_tid_t thread_id = k_thread_create(&thread_data, thread_stack,
                                        K_THREAD_STACK_SIZEOF(thread_stack),
                                        thread_function,
                                        test_settings, NULL, NULL,
                                        TEST_THREAD_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(thread_id, "test_thread");
    k_thread_start(thread_id);

    //wait for the test to finish
    k_sem_take(&test_sem, K_FOREVER);

    //make sure the thread is stopped
    k_thread_abort(thread_id);  
}

#endif // CONFIG_COAP_TWT_TESTBED_SERVER