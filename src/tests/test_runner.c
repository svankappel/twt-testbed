#include "test_runner.h"

#include "test_sensor_ps.h"
#include "test_sensor_twt.h"

K_SEM_DEFINE(test_sem, 0, 1);



void run_tests(){

    //PS Tests
    {
    #ifdef CONFIG_PS_TESTS_ENABLE

    #ifdef CONFIG_PS_TEST_1
    struct test_sensor_ps_settings test_settings_1 = {

            .iterations = CONFIG_PS_TEST_1_ITERATIONS,
            .send_interval = CONFIG_PS_TEST_1_SEND_INTERVAL,
            .test_id = 1,

            #ifdef CONFIG_PS_TEST_1_PS_ENABLED
            .ps_enabled = PS_MODE_ENABLED,
            #else
            .ps_enabled = PS_MODE_DISABLED,
            #endif //CONFIG_PS_TEST_1_PS_ENABLED

            #ifndef CONFIG_PS_TEST_1_MODE_WMM
            .ps_mode = PS_MODE_LEGACY,
            #else
            .ps_mode = PS_MODE_WMM,
            #endif //CONFIG_PS_TEST_1_MODE_WMM

            #ifndef CONFIG_PS_TEST_1_WAKEUP_MODE_LISTENINTERVAL
            .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
            #else
            .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
            #endif //CONFIG_PS_TEST_1_WAKEUP_MODE_LISTENINTERVAL
    };
    test_sensor_ps(&test_sem, &test_settings_1);
    #endif //CONFIG_PS_TEST_1

    #ifdef CONFIG_PS_TEST_2
    struct test_sensor_ps_settings test_settings_2 = {
            .iterations = CONFIG_PS_TEST_2_ITERATIONS,
            .send_interval = CONFIG_PS_TEST_2_SEND_INTERVAL,
            .test_id = 2,

            #ifdef CONFIG_PS_TEST_2_PS_ENABLED
            .ps_enabled = PS_MODE_ENABLED,
            #else
            .ps_enabled = PS_MODE_DISABLED,
            #endif //CONFIG_PS_TEST_2_PS_ENABLED

            #ifndef CONFIG_PS_TEST_2_MODE_WMM
            .ps_mode = PS_MODE_LEGACY,
            #else
            .ps_mode = PS_MODE_WMM,
            #endif //CONFIG_PS_TEST_2_MODE_WMM

            #ifndef CONFIG_PS_TEST_2_WAKEUP_MODE_LISTENINTERVAL
            .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
            #else
            .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
            #endif //CONFIG_PS_TEST_2_WAKEUP_MODE_LISTENINTERVAL
    };
    test_sensor_ps(&test_sem, &test_settings_2);
    #endif //CONFIG_PS_TEST_2

    #ifdef CONFIG_PS_TEST_3
    struct test_sensor_ps_settings test_settings_3 = {
            .iterations = CONFIG_PS_TEST_3_ITERATIONS,
            .send_interval = CONFIG_PS_TEST_3_SEND_INTERVAL,
            .test_id = 3,

            #ifdef CONFIG_PS_TEST_3_PS_ENABLED
            .ps_enabled = PS_MODE_ENABLED,
            #else
            .ps_enabled = PS_MODE_DISABLED,
            #endif //CONFIG_PS_TEST_3_PS_ENABLED

            #ifndef CONFIG_PS_TEST_3_MODE_WMM
            .ps_mode = PS_MODE_LEGACY,
            #else
            .ps_mode = PS_MODE_WMM,
            #endif //CONFIG_PS_TEST_3_MODE_WMM

            #ifndef CONFIG_PS_TEST_3_WAKEUP_MODE_LISTENINTERVAL
            .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
            #else
            .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
            #endif //CONFIG_PS_TEST_3_WAKEUP_MODE_LISTENINTERVAL
    };
    test_sensor_ps(&test_sem, &test_settings_3);
    #endif //CONFIG_PS_TEST_3

    #ifdef CONFIG_PS_TEST_4
    struct test_sensor_ps_settings test_settings_4 = {
            .iterations = CONFIG_PS_TEST_4_ITERATIONS,
            .send_interval = CONFIG_PS_TEST_4_SEND_INTERVAL,
            .test_id = 4,

            #ifdef CONFIG_PS_TEST_4_PS_ENABLED
            .ps_enabled = PS_MODE_ENABLED,
            #else
            .ps_enabled = PS_MODE_DISABLED,
            #endif //CONFIG_PS_TEST_4_PS_ENABLED

            #ifndef CONFIG_PS_TEST_4_MODE_WMM
            .ps_mode = PS_MODE_LEGACY,
            #else
            .ps_mode = PS_MODE_WMM,
            #endif //CONFIG_PS_TEST_4_MODE_WMM

            #ifndef CONFIG_PS_TEST_4_WAKEUP_MODE_LISTENINTERVAL
            .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
            #else
            .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
            #endif //CONFIG_PS_TEST_4_WAKEUP_MODE_LISTENINTERVAL
    };
    test_sensor_ps(&test_sem, &test_settings_4);
    #endif //CONFIG_PS_TEST_4

    #ifdef CONFIG_PS_TEST_5
    struct test_sensor_ps_settings test_settings_5 = {
            .iterations = CONFIG_PS_TEST_5_ITERATIONS,
            .send_interval = CONFIG_PS_TEST_5_SEND_INTERVAL,
            .test_id = 5,

            #ifdef CONFIG_PS_TEST_5_PS_ENABLED
            .ps_enabled = PS_MODE_ENABLED,
            #else
            .ps_enabled = PS_MODE_DISABLED,
            #endif //CONFIG_PS_TEST_5_PS_ENABLED

            #ifndef CONFIG_PS_TEST_5_MODE_WMM
            .ps_mode = PS_MODE_LEGACY,
            #else
            .ps_mode = PS_MODE_WMM,
            #endif //CONFIG_PS_TEST_5_MODE_WMM

            #ifndef CONFIG_PS_TEST_5_WAKEUP_MODE_LISTENINTERVAL
            .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
            #else
            .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
            #endif //CONFIG_PS_TEST_5_WAKEUP_MODE_LISTENINTERVAL
    };
    test_sensor_ps(&test_sem, &test_settings_5);
    #endif //CONFIG_PS_TEST_5

    #endif //CONFIG_PS_TESTS_ENABLE
    }

    {
    #ifdef CONFIG_TWT_TESTS_ENABLE

    #ifdef CONFIG_TWT_TEST_1
    struct test_sensor_twt_settings test_settings_1 = {
            .iterations = CONFIG_TWT_TEST_1_ITERATIONS,
            .twt_interval = CONFIG_TWT_TEST_1_INTERVAL,
            .twt_wake_interval = CONFIG_TWT_TEST_1_SESSION_DURATION,
            .test_id = 1,
            .wake_ahead_ms = 100,
    };
    test_sensor_twt(&test_sem, &test_settings_1);
    #endif //CONFIG_TWT_TEST_1
    
    #ifdef CONFIG_TWT_TEST_2
    struct test_sensor_twt_settings test_settings_2 = {
            .iterations = CONFIG_TWT_TEST_2_ITERATIONS,
            .twt_interval = CONFIG_TWT_TEST_2_INTERVAL,
            .twt_wake_interval = CONFIG_TWT_TEST_2_SESSION_DURATION,
            .test_id = 2,
            .wake_ahead_ms = 100,
    };
    test_sensor_twt(&test_sem, &test_settings_2);
    #endif //CONFIG_TWT_TEST_2

    #ifdef CONFIG_TWT_TEST_3
    struct test_sensor_twt_settings test_settings_3 = {
            .iterations = CONFIG_TWT_TEST_3_ITERATIONS,
            .twt_interval = CONFIG_TWT_TEST_3_INTERVAL,
            .twt_wake_interval = CONFIG_TWT_TEST_3_SESSION_DURATION,
            .test_id = 3,
            .wake_ahead_ms = 100,
    };
    test_sensor_twt(&test_sem, &test_settings_3);
    #endif //CONFIG_TWT_TEST_3

    #ifdef CONFIG_TWT_TEST_4
    struct test_sensor_twt_settings test_settings_4 = {
            .iterations = CONFIG_TWT_TEST_4_ITERATIONS,
            .twt_interval = CONFIG_TWT_TEST_4_INTERVAL,
            .twt_wake_interval = CONFIG_TWT_TEST_4_SESSION_DURATION,
            .test_id = 4,
            .wake_ahead_ms = 100,
    };
    test_sensor_twt(&test_sem, &test_settings_4);
    #endif //CONFIG_TWT_TEST_4

    #ifdef CONFIG_TWT_TEST_5
    struct test_sensor_twt_settings test_settings_5 = {
            .iterations = CONFIG_TWT_TEST_5_ITERATIONS,
            .twt_interval = CONFIG_TWT_TEST_5_INTERVAL,
            .twt_wake_interval = CONFIG_TWT_TEST_5_SESSION_DURATION,
            .test_id = 5,
            .wake_ahead_ms = 100,
    };
    test_sensor_twt(&test_sem, &test_settings_5);
    #endif //CONFIG_TWT_TEST_5

    #endif //CONFIG_TWT_TESTS_ENABLE
    }

}