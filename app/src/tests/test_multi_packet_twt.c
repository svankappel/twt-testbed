#ifdef CONFIG_COAP_TWT_TESTBED_SERVER

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
    int received_serv;
    uint32_t latency_sum;
    uint16_t latency_hist[MAX_INTERVALS_BUFFERED];
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
            "=                       TEST RESULTS - MULTI PACKET TWT                        =\n"
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
            monitor.iter,
            wifi_twt_get_interval_ms() / 1000,
            wifi_twt_get_wake_interval_ms(),
            monitor.sent,
            monitor.received_serv,
            monitor.received,
            monitor.received_serv < 0 ? -1 : monitor.sent - monitor.received_serv,
            monitor.received_serv < 0 ? -1 : monitor.received_serv - monitor.received,
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
                ret = coap_put(TESTBED_SENSOR_RESOURCE, buf);
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

    // Extract the semaphore and test settings
    struct k_sem *test_sem = (struct k_sem *)arg1;
    memcpy(&test_settings, arg2, sizeof(test_settings));

    LOG_INF("Starting test %d setup", test_settings.test_id);

    int ret;

    // connect to wifi
    ret = wifi_connect();
    if(ret != 0){
        LOG_ERR("Failed to connect to wifi");
        k_sleep(K_FOREVER);
    }
    wifi_register_disconnected_cb(wifi_disconnected_event);
    k_sleep(K_SECONDS(5));

    // coap
    coap_register_put_response_callback(handle_coap_response);
    ret = coap_validate();
    if(ret != 0){
        LOG_ERR("Failed to validate CoAP client");
        k_sleep(K_FOREVER);
    }
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

        //get stat from server
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

    k_sleep(K_SECONDS(2)); //give time for the logs to print

    // give the semaphore to start the next test
    k_sem_give(test_sem);
}

// Function to initialize the test
void test_multi_packet_twt(struct k_sem *sem, void * test_settings) {
    
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

#endif // CONFIG_COAP_TWT_TESTBED_SERVER