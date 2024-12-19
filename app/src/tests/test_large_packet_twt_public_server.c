#ifndef CONFIG_COAP_TWT_TESTBED_SERVER

#include "test_large_packet_twt.h"
#include "test_global.h"
#include "test_report.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/random/random.h>

#include "wifi_sta.h"
#include "wifi_ps.h"
#include "wifi_twt.h"
#include "coap.h"

#ifdef CONFIG_PROFILER_ENABLE
#include "profiler.h"
#endif //CONFIG_PROFILER_ENABLE

LOG_MODULE_REGISTER(test_large_packet_twt, CONFIG_MY_TEST_LOG_LEVEL);

#define MAX_INTERVALS_BUFFERED 50

static struct test_large_packet_twt_settings test_settings;

static K_SEM_DEFINE(wake_ahead_sem, 0, 1);

struct test_control{
    bool test_failed;
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

static void print_test_results() {
    // Check for inconsistencies and print warnings
    if ((monitor.iter != test_settings.iterations)) {
        LOG_WRN("Warning: Test could not complete all iterations");
    }

    // Print the results
    LOG_INF("\n\n"
            "================================================================================\n"
            "=                       TEST RESULTS - LARGE PACKET TWT                        =\n"
            "================================================================================\n"
            "=  Test setup                                                                  =\n"
            "================================================================================\n"
            "=  Test Number:                           %6d                               =\n"
            "=  Iterations:                            %6d                               =\n"
            "=------------------------------------------------------------------------------=\n"
            "=  Request payload size (bytes):          %6d                               =\n"
            "=------------------------------------------------------------------------------=\n"
            "=  Negotiated TWT Interval:               %6d s                             =\n"
            "=  Negotiated TWT Wake Interval:          %6d ms                            =\n"
            "================================================================================\n"
            "=  Stats                                                                       =\n"
            "================================================================================\n"
            "=  Requests sent:                         %6d                               =\n"
            "-------------------------------------------------------------------------------=\n"
            "=  Responses received:                    %6d                               =\n"
            "=------------------------------------------------------------------------------=\n"
            "=  Average latency:                       %6d s                             =\n"
            "================================================================================\n",
            test_settings.test_id,
            monitor.iter,
            test_settings.bytes,
            wifi_twt_get_interval_ms() / 1000,
            wifi_twt_get_wake_interval_ms(),
            monitor.sent,
            monitor.received,
            monitor.received == 0 ? -1 : monitor.latency_sum/monitor.received);
    

    // Print the latency histogram
    char hist_str[1024] = {0};
    char temp[32];
    for (int i = 0; i < MAX_INTERVALS_BUFFERED; i++) {
        if(monitor.latency_hist[i] != 0){
            snprintf(temp, sizeof(temp), "%d;%d\n", i * test_settings.twt_interval / 1000, monitor.latency_hist[i]);
            strncat(hist_str, temp, sizeof(hist_str) - strlen(hist_str) - 1);
        } 
    }
    snprintf(temp, sizeof(temp), "lost;%d\n", monitor.sent - monitor.received);
    strncat(hist_str, temp, sizeof(hist_str) - strlen(hist_str) - 1);
    LOG_INF("\n================================================================================\n"
                "=  Latency Histogram                                                           =\n"
                "================================================================================\n"
                "%s"
                "================================================================================\n",
                hist_str);
}

static void generate_test_report() {
    struct test_report report;
    sprintf(report.test_title, "\"test_title\":\"Large Packet Use Case - TWT\"");

    sprintf(report.test_setup,
            "\"test_setup\":\n"
            "{\n"
            "\"Iterations\": %d,\n"
            "\"Request_Payload_Size\": \"%d bytes\",\n"
            "\"Negotiated_TWT_Interval\": \"%d s\",\n"
            "\"Negotiated_TWT_Wake_Interval\": \"%d ms\"\n"
            "}",
            monitor.iter,
            test_settings.bytes,
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

    control.test_failed = true;
    k_sem_give(&wake_ahead_sem);
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

    int index = (time+(test_settings.twt_interval/2)) / test_settings.twt_interval;
    if (index < MAX_INTERVALS_BUFFERED) {
        monitor.latency_hist[index]++;
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

        if(control.test_failed){
            break;
        }

        int ret=0;

        if(monitor.iter < test_settings.iterations){

            // Generate an array with random chars
            char random_data[test_settings.bytes-21];
            for (int i = 0; i < test_settings.bytes-21; i++) {
                if(i%100 == 0 || i == test_settings.bytes-22){
                    random_data[i] = '\n'; 
                }else{
                    random_data[i] = 'a' + (sys_rand32_get() % 26); // Random char
                }
            }
            char buf[test_settings.bytes+20];

            sprintf(buf, "/%06d/%s/largeupload/", monitor.iter++,random_data);
            
            ret = coap_put(CONFIG_COAP_SENSOR_TEST_RESOURCE, buf);

            if(ret >= 0){
                monitor.sent++;
            } 
        }else{
            break;
        }
    }
    k_sleep(K_MSEC(test_settings.twt_interval*2));
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
    ret = wifi_connect();
    if(ret != 0){
        LOG_ERR("Failed to connect to wifi");
        k_sleep(K_FOREVER);
    }
    wifi_register_disconnected_cb(wifi_disconnected_event);
    k_sleep(K_SECONDS(5));


    //coap
    coap_register_put_response_callback(handle_coap_response);
    coap_init_pool(test_settings.twt_interval * MAX_INTERVALS_BUFFERED);  // init coap request pool with 1s request timeout
    coap_put(CONFIG_COAP_SENSOR_TEST_RESOURCE, "{init-message}"); //send one message before TWT
    k_sleep(K_SECONDS(2));

    // TWT
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

    if(!control.test_failed){
        LOG_INF("Test %d finished", test_settings.test_id);

        //coap
        coap_register_put_response_callback(NULL);

        // tear down TWT and disconnect from wifi
        if(wifi_twt_is_enabled()){
            wifi_twt_teardown();
        }
        k_sleep(K_SECONDS(2));
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

    print_test_results();

    generate_test_report();

    k_sleep(K_SECONDS(2)); //give time for the logs to print

    // give the semaphore to start the next test
    k_sem_give(test_sem);
}

// Function to initialize the test
void test_large_packet_twt(struct k_sem *sem, void * test_settings) {
    
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