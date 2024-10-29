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


struct test_sensor_twt_settings test_settings;

K_SEM_DEFINE(end_sem, 0, 1);
K_SEM_DEFINE(wake_ahead_sem, 0, 1);

struct test_control{
    int iter;
    int sent;
    int received;
    int send_fails;
    int send_err_11;
    int send_err_120;
    int send_err_other;
    int recv_resp;
    int recv_err;
    int recv_serv;
};

void print_test_results(struct test_control *control) {
    // Check for inconsistencies and print warnings
    if ((control->send_err_11 + control->send_err_120 + control->send_err_other) != control->send_fails) {
        LOG_WRN("Warning: Sum of send errors does not match send fails");
    }
    if (control->sent != control->received) {
        LOG_WRN("Warning: Sent messages do not match received messages");
    }
    if ((control->recv_resp + control->recv_err) != control->received) {
        LOG_WRN("Warning: Sum of received responses and errors does not match received messages");
    }
    if ((control->sent + control->send_fails) != control->iter) {
        LOG_WRN("Warning: Sent messages plus send fails do not match iterations");
    }
    if (control->recv_serv < 0) {
        LOG_WRN("Warning: Could not receive server stats");
    }


    // Print the results
    LOG_INF("\n\n"
            "================================================================================\n"
            "=                                 TEST RESULTS                                 =\n"
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
            "=  Request timed-out:                     %6d                               =\n"
            "=------------------------------------------------------------------------------=\n"
            "=  Requests lost:                         %6d                               =\n"
            "=  Responses lost:                        %6d                               =\n"
            "================================================================================\n"
            "=  Errors                                                                      =\n"
            "================================================================================\n"
            "=  Send Fails:                            %6d                               =\n"
            "=    Send Error -11:                      %6d                               =\n"
            "=    Send Error -120:                     %6d                               =\n"
            "=    Other Send Errors:                   %6d                               =\n"
            "================================================================================\n",
            test_settings.test_number,
            control->iter,
            wifi_twt_get_interval_ms() / 1000,
            wifi_twt_get_wake_interval_ms(),
            control->sent,
            control->recv_serv,
            control->recv_resp,
            control->recv_err,
            control->sent - control->recv_serv,
            control->recv_serv - control->recv_resp,
            control->send_fails,
            control->send_err_11,
            control->send_err_120,
            control->send_err_other);
}

//--------------------------------------------------------------------     
// Callback function to handle TWT session wake ahead event
//--------------------------------------------------------------------
void handle_twt_event()
{
    k_sem_give(&wake_ahead_sem);
}

//--------------------------------------------------------------------     
// Callback function to handle coap responses
//--------------------------------------------------------------------
void handle_coap_response(int16_t code, void * user_data)
{
    struct test_control * control = (struct test_control *)user_data;

    control->received++;

    if(code >= 0){
        control->recv_resp++;
    }else{
        control->recv_err++;
    }

    if(control->iter>=test_settings.iterations && (control->received == control->sent))
    {
        k_sem_give(&end_sem);
    }
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
    wifi_twt_setup(test_settings.twt_wake_interval, test_settings.twt_interval);
}

//--------------------------------------------------------------------
// Function to run the test
//--------------------------------------------------------------------
void run_test(struct test_control * control)
{
    while(true)
    {
        k_sem_take(&wake_ahead_sem, K_FOREVER);

        char buf[32];
        int ret;

        if(control->iter < test_settings.iterations)
        {
            sprintf(buf, "{\"sensor-value\":%d}", control->iter++);
            ret = coap_put("test", buf, test_settings.request_timeout);
            if(ret == 0){
                control->sent++;
            } else if(ret == -11){
                control->send_err_11++;
                control->send_fails++;
            }else if(ret == -120){  
                control->send_err_120++;
                control->send_fails++;
            }else{
                control->send_err_other++;
                control->send_fails++;
            }
            
        }else{
            break;
        }
    }
    k_sem_take(&end_sem, K_FOREVER);
}
//--------------------------------------------------------------------


// Thread function that runs the test
void thread_function(void *arg1, void *arg2, void *arg3) 
{
    struct test_control control = { 0 };

    // Extract the semaphore and test settings
    struct k_sem *start_sem = (struct k_sem *)arg1;
    memcpy(&test_settings, arg2, sizeof(test_settings));

    // Wait for the semaphore to start the test
    k_sem_take(start_sem, K_FOREVER);

    LOG_INF("Starting test %d setup", test_settings.test_number);

    //register coap response callback
    coap_register_response_callback(handle_coap_response,(void*)&control);

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

//    ret = coap_validate();
    if(ret != 0)
    {
        LOG_ERR("Failed to validate CoAP client");
        k_sleep(K_FOREVER);
    }
    LOG_DBG("CoAP validated");

    k_sleep(K_SECONDS(1));

    // configure TWT
    wifi_twt_register_event_callback(handle_twt_event,test_settings.wake_ahead_ms);
    configure_twt(&test_settings);
    LOG_DBG("TWT configured");

    // run the test
    LOG_INF("Starting test sensor TWT %d", test_settings.test_number);
    profiler_output_binary(test_settings.test_number);

    run_test(&control);

    profiler_all_clear();
    LOG_INF("Test sensor TWT %d finished", test_settings.test_number);

    // tear down TWT and disconnect from wifi
    if(wifi_twt_is_enabled())
    {
        wifi_twt_teardown();
    }

    k_sleep(K_SECONDS(1));

    control.recv_serv = coap_get_stat();
    if(control.recv_serv < 0)
    {
        LOG_WRN("Failed to get CoAP stats from server");
    }

    ret = wifi_disconnect();
    if(ret != 0)
    {
        LOG_ERR("Failed to disconnect from wifi");
        k_sleep(K_FOREVER);
    }

    print_test_results(&control);

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
