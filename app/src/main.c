

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/random/random.h>

#include "wifi_sta.h"
#include "wifi_ps.h"
#include "wifi_twt.h"

#ifdef CONFIG_COAP_SECURE
#include "coap_security.h"
#endif //CONFIG_COAP_SECURE

#include "coap.h"

#ifdef CONFIG_PROFILER_ENABLE
#include "profiler.h"
#endif //CONFIG_PROFILER_ENABLE

#include "test_runner.h"


LOG_MODULE_REGISTER(main, CONFIG_MY_MAIN_LOG_LEVEL);

static void check_config()
{
    //check if the CoAP server port is correct
    #ifndef CONFIG_COAP_SECURE
    if(CONFIG_COAP_TEST_SERVER_PORT != 5683){
        LOG_WRN("CoAP/UDP port is 5683, got %d", CONFIG_COAP_TEST_SERVER_PORT);
    }
    #else
    if(CONFIG_COAP_TEST_SERVER_PORT != 5684){
        LOG_WRN("CoAP/DTLS port is 5684, got %d", CONFIG_COAP_TEST_SERVER_PORT);
    }
    #endif //CONFIG_COAP_SECURE

    //check if the CoAP server hostname is correct
    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
    if(strcmp(CONFIG_COAP_TEST_SERVER_HOSTNAME, "californium.eclipseprojects.io") != 0){
        LOG_WRN("Californium server is not compatible with TWT testbed private server features, remove CONFIG_COAP_TWT_TESTBED_SERVER to use public server");
    }
    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
}


int main(void)
{
    int ret;

    LOG_INF("Starting TWT testbed ...");

    //check configuration
    check_config(); 

    // initialize setup

    #ifdef CONFIG_PROFILER_ENABLE
    ret = profiler_init();
    if(ret != 0)
    {
        LOG_ERR("Failed to initialize profiler");
        k_sleep(K_FOREVER);
    }
    #endif //CONFIG_PROFILER_ENABLE

    //configure DTLS credentials
    #ifdef CONFIG_COAP_SECURE
    ret = configure_psk();
    if(ret != 0)
    {
        LOG_ERR("Failed to configure PSK");
        k_sleep(K_FOREVER);
    }
    #endif //CONFIG_COAP_SECURE

    wifi_init();

    wifi_ps_set_listen_interval(CONFIG_PS_LISTEN_INTERVAL);

    ret = wifi_connect();
    if(ret != 0)
    {
        LOG_ERR("Failed to connect to wifi");
        k_sleep(K_FOREVER);
    }

    
    coap_init();
    if(ret != 0)
    {
        LOG_ERR("Failed to initialize CoAP client");
        k_sleep(K_FOREVER);
    }

    k_sleep(K_SECONDS(1));

 
    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
    ret = coap_validate();
    if(ret != 0)
    {
        LOG_ERR("Failed to validate CoAP client");
        k_sleep(K_FOREVER);
    }
    #endif //CONFIG_COAP_TWT_TESTBED_SERVER

    ret = wifi_disconnect();
    if(ret != 0)
    {
        LOG_ERR("Failed to disconnect from wifi");
        k_sleep(K_FOREVER);
     }

    k_sleep(K_SECONDS(1));

    LOG_INF("TWT testbed initialized. Running tests ...");



    //run tests

    run_tests();


    LOG_INF("Tests Finished!");


    k_sleep(K_FOREVER);
    return 0;
}
