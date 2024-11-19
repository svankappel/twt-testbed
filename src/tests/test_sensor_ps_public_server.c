#ifndef CONFIG_COAP_TWT_TESTBED_SERVER

#include "test_sensor_ps.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include "wifi_sta.h"
#include "wifi_ps.h"
#include "coap.h"
#include "profiler.h"
LOG_MODULE_REGISTER(test_sensor_ps, CONFIG_MY_TEST_LOG_LEVEL);

#define STACK_SIZE 4096
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
    int send_fails;
    int send_err_11;
    int send_err_120;
    int send_err_other;
    int recv_resp;
    int recv_err;
    int recv_serv;
};

static void print_test_results(struct test_control *control) {
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
            "=                           TEST RESULTS - SENSOR PS                           =\n"
            "================================================================================\n"
            "=  Test setup                                                                  =\n"
            "================================================================================\n"
            "=  Test Number:                           %6d                               =\n"
            "=  Iterations:                            %6d                               =\n"
            "-------------------------------------------------------------------------------=\n"
            "=  Power Save:                          %s                               =\n"
            "=  Mode:                                  %s                               =\n"
            "=  Wake-Up mode:                 %s                               =\n"
            "=  Listen Interval:                       %6d                               =\n"
            "================================================================================\n"
            "=  Stats                                                                       =\n"
            "================================================================================\n"
            "=  Requests sent:                         %6d                               =\n"
            "-------------------------------------------------------------------------------=\n"
            "=  Responses received:                    %6d                               =\n"
            "=  Request timed-out:                     %6d                               =\n"
            "================================================================================\n"
            "=  Errors                                                                      =\n"
            "================================================================================\n"
            "=  Send Fails:                            %6d                               =\n"
            "=    Send Error -11:                      %6d                               =\n"
            "=    Send Error -120:                     %6d                               =\n"
            "=    Other Send Errors:                   %6d                               =\n"
            "================================================================================\n",
            test_settings.test_id,
            control->iter,
            test_settings.ps_enabled ? " Enabled" : "Disabled",
            test_settings.ps_mode ? "   WMM" : "Legacy",
            test_settings.ps_wakeup_mode ? "Listen Interval" : "           DTIM",
            CONFIG_PS_LISTEN_INTERVAL,
            control->sent,
            control->recv_resp,
            control->recv_err,
            control->send_fails,
            control->send_err_11,
            control->send_err_120,
            control->send_err_other);
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
static void handle_coap_response(int16_t code, void * user_data)
{
    if(test_failed){
        return;
    }
    struct test_control * control = (struct test_control *)user_data;

    control->received++;

    if(code == 0x44){
        control->recv_resp++;
    }else{
        control->recv_err++;
    }

    if((control->iter>=test_settings.iterations) && (control->received == control->sent))
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

    if(test_settings.ps_enabled == PS_MODE_ENABLED){
        wifi_ps_enable();
    }else{
        wifi_ps_disable();
    }
}

//--------------------------------------------------------------------
// Function to run the test
//--------------------------------------------------------------------
static void run_test(struct test_control * control)
{
    k_timer_start(&send_timer, K_MSEC(test_settings.send_interval), K_NO_WAIT);

    while(true){
        k_sem_take(&timer_event_sem, K_FOREVER);

        if(test_failed){
            k_sleep(K_SECONDS(10));
            break;
        }

        char buf[32];
        int ret;

        if(control->iter < test_settings.iterations){
            sprintf(buf, "{\"sensor-value\":%d}", control->iter++);
            ret = coap_put(CONFIG_COAP_TEST_RESOURCE, buf, test_settings.send_interval+1000);
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
static void thread_function(void *arg1, void *arg2, void *arg3) 
{
    struct test_control control = { 0 };

    // Extract the semaphore and test settings
    struct k_sem *test_sem = (struct k_sem *)arg1;
    memcpy(&test_settings, arg2, sizeof(test_settings));

    LOG_INF("Starting test %d setup", test_settings.test_id);

    //register coap response callback
    coap_register_response_callback(handle_coap_response,(void*)&control);

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


    // run the test
    LOG_INF("Starting test %d", test_settings.test_id);
    profiler_output_binary(test_settings.test_id);

    run_test(&control);

    profiler_all_clear();

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
        control.recv_serv = -1;
    }

    print_test_results(&control);

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