

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/random/random.h>

#include "wifi_sta.h"
#include "wifi_ps.h"
#include "wifi_twt.h"

#include "coap.h"

#include "profiler.h"

#include "test_runner.h"


LOG_MODULE_REGISTER(main, CONFIG_MY_MAIN_LOG_LEVEL);


int main(void)
{
    int ret;

    LOG_INF("Starting TWT testbed ...");

    // initialize setup
    ret = profiler_init();
    if(ret != 0)
    {
        LOG_ERR("Failed to initialize profiler");
        k_sleep(K_FOREVER);
    }

    wifi_init();

    wifi_ps_set_listen_interval(CONFIG_PS_LISTEN_INTERVAL);

    ret = wifi_ps_disable();
    if(ret != 0)
    {
        LOG_ERR("Failed to disable power save mode");
        k_sleep(K_FOREVER);
    }

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
