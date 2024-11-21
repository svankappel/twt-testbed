#include "test_runner.h"

#include "test_sensor_ps.h"
#include "test_sensor_twt.h"

#include "test_large_packet_ps.h"
#include "test_large_packet_twt.h"

K_SEM_DEFINE(test_sem, 0, 1);



void run_tests(){

//sensor tests
#ifdef CONFIG_SENSOR_TESTS_ENABLE

//PS Tests
{
#ifdef CONFIG_SENSOR_PS_TESTS_ENABLE

#ifdef CONFIG_SENSOR_PS_TEST_1
struct test_sensor_ps_settings test_settings_1 = {

                .iterations = CONFIG_SENSOR_PS_TEST_1_ITERATIONS,
                .send_interval = CONFIG_SENSOR_PS_TEST_1_SEND_INTERVAL,
                .test_id = 1,

                #ifdef CONFIG_SENSOR_PS_TEST_1_PS_ENABLED
                .ps_enabled = PS_MODE_ENABLED,
                #else
                .ps_enabled = PS_MODE_DISABLED,
                #endif //CONFIG_SENSOR_PS_TEST_1_PS_ENABLED

                #ifndef CONFIG_SENSOR_PS_TEST_1_MODE_WMM
                .ps_mode = PS_MODE_LEGACY,
                #else
                .ps_mode = PS_MODE_WMM,
                #endif //CONFIG_SENSOR_PS_TEST_1_MODE_WMM

                #ifndef CONFIG_SENSOR_PS_TEST_1_WAKEUP_MODE_LISTENINTERVAL
                .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                #else
                .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                #endif //CONFIG_SENSOR_PS_TEST_1_WAKEUP_MODE_LISTENINTERVAL
};
test_sensor_ps(&test_sem, &test_settings_1);
#endif //CONFIG_SENSOR_PS_TEST_1

#ifdef CONFIG_SENSOR_PS_TEST_2
struct test_sensor_ps_settings test_settings_2 = {
                .iterations = CONFIG_SENSOR_PS_TEST_2_ITERATIONS,
                .send_interval = CONFIG_SENSOR_PS_TEST_2_SEND_INTERVAL,
                .test_id = 2,

                #ifdef CONFIG_SENSOR_PS_TEST_2_PS_ENABLED
                .ps_enabled = PS_MODE_ENABLED,
                #else
                .ps_enabled = PS_MODE_DISABLED,
                #endif //CONFIG_SENSOR_PS_TEST_2_PS_ENABLED

                #ifndef CONFIG_SENSOR_PS_TEST_2_MODE_WMM
                .ps_mode = PS_MODE_LEGACY,
                #else
                .ps_mode = PS_MODE_WMM,
                #endif //CONFIG_SENSOR_PS_TEST_2_MODE_WMM

                #ifndef CONFIG_SENSOR_PS_TEST_2_WAKEUP_MODE_LISTENINTERVAL
                .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                #else
                .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                #endif //CONFIG_SENSOR_PS_TEST_2_WAKEUP_MODE_LISTENINTERVAL
};
test_sensor_ps(&test_sem, &test_settings_2);
#endif //CONFIG_SENSOR_PS_TEST_2

#ifdef CONFIG_SENSOR_PS_TEST_3
struct test_sensor_ps_settings test_settings_3 = {
                .iterations = CONFIG_SENSOR_PS_TEST_3_ITERATIONS,
                .send_interval = CONFIG_SENSOR_PS_TEST_3_SEND_INTERVAL,
                .test_id = 3,

                #ifdef CONFIG_SENSOR_PS_TEST_3_PS_ENABLED
                .ps_enabled = PS_MODE_ENABLED,
                #else
                .ps_enabled = PS_MODE_DISABLED,
                #endif //CONFIG_SENSOR_PS_TEST_3_PS_ENABLED

                #ifndef CONFIG_SENSOR_PS_TEST_3_MODE_WMM
                .ps_mode = PS_MODE_LEGACY,
                #else
                .ps_mode = PS_MODE_WMM,
                #endif //CONFIG_SENSOR_PS_TEST_3_MODE_WMM

                #ifndef CONFIG_SENSOR_PS_TEST_3_WAKEUP_MODE_LISTENINTERVAL
                .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                #else
                .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                #endif //CONFIG_SENSOR_PS_TEST_3_WAKEUP_MODE_LISTENINTERVAL
};
test_sensor_ps(&test_sem, &test_settings_3);
#endif //CONFIG_SENSOR_PS_TEST_3

#ifdef CONFIG_SENSOR_PS_TEST_4
struct test_sensor_ps_settings test_settings_4 = {
                .iterations = CONFIG_SENSOR_PS_TEST_4_ITERATIONS,
                .send_interval = CONFIG_SENSOR_PS_TEST_4_SEND_INTERVAL,
                .test_id = 4,

                #ifdef CONFIG_SENSOR_PS_TEST_4_PS_ENABLED
                .ps_enabled = PS_MODE_ENABLED,
                #else
                .ps_enabled = PS_MODE_DISABLED,
                #endif //CONFIG_SENSOR_PS_TEST_4_PS_ENABLED

                #ifndef CONFIG_SENSOR_PS_TEST_4_MODE_WMM
                .ps_mode = PS_MODE_LEGACY,
                #else
                .ps_mode = PS_MODE_WMM,
                #endif //CONFIG_SENSOR_PS_TEST_4_MODE_WMM

                #ifndef CONFIG_SENSOR_PS_TEST_4_WAKEUP_MODE_LISTENINTERVAL
                .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                #else
                .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                #endif //CONFIG_SENSOR_PS_TEST_4_WAKEUP_MODE_LISTENINTERVAL
};
test_sensor_ps(&test_sem, &test_settings_4);
#endif //CONFIG_SENSOR_PS_TEST_4

#ifdef CONFIG_SENSOR_PS_TEST_5
struct test_sensor_ps_settings test_settings_5 = {
                .iterations = CONFIG_SENSOR_PS_TEST_5_ITERATIONS,
                .send_interval = CONFIG_SENSOR_PS_TEST_5_SEND_INTERVAL,
                .test_id = 5,

                #ifdef CONFIG_SENSOR_PS_TEST_5_PS_ENABLED
                .ps_enabled = PS_MODE_ENABLED,
                #else
                .ps_enabled = PS_MODE_DISABLED,
                #endif //CONFIG_SENSOR_PS_TEST_5_PS_ENABLED

                #ifndef CONFIG_SENSOR_PS_TEST_5_MODE_WMM
                .ps_mode = PS_MODE_LEGACY,
                #else
                .ps_mode = PS_MODE_WMM,
                #endif //CONFIG_SENSOR_PS_TEST_5_MODE_WMM

                #ifndef CONFIG_SENSOR_PS_TEST_5_WAKEUP_MODE_LISTENINTERVAL
                .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                #else
                .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                #endif //CONFIG_SENSOR_PS_TEST_5_WAKEUP_MODE_LISTENINTERVAL
};
test_sensor_ps(&test_sem, &test_settings_5);
#endif //CONFIG_SENSOR_PS_TEST_5

#endif //CONFIG_SENSOR_PS_TESTS_ENABLE
}

{
#ifdef CONFIG_SENSOR_TWT_TESTS_ENABLE

#ifdef CONFIG_SENSOR_TWT_TEST_1
struct test_sensor_twt_settings test_settings_1 = {
                .iterations = CONFIG_SENSOR_TWT_TEST_1_ITERATIONS,
                .twt_interval = CONFIG_SENSOR_TWT_TEST_1_INTERVAL,
                .twt_wake_interval = CONFIG_SENSOR_TWT_TEST_1_SESSION_DURATION,
                .test_id = 1,
                .wake_ahead_ms = 100,
};
test_sensor_twt(&test_sem, &test_settings_1);
#endif //CONFIG_SENSOR_TWT_TEST_1

#ifdef CONFIG_SENSOR_TWT_TEST_2
struct test_sensor_twt_settings test_settings_2 = {
                .iterations = CONFIG_SENSOR_TWT_TEST_2_ITERATIONS,
                .twt_interval = CONFIG_SENSOR_TWT_TEST_2_INTERVAL,
                .twt_wake_interval = CONFIG_SENSOR_TWT_TEST_2_SESSION_DURATION,
                .test_id = 2,
                .wake_ahead_ms = 100,
};
test_sensor_twt(&test_sem, &test_settings_2);
#endif //CONFIG_SENSOR_TWT_TEST_2

#ifdef CONFIG_SENSOR_TWT_TEST_3
struct test_sensor_twt_settings test_settings_3 = {
                .iterations = CONFIG_SENSOR_TWT_TEST_3_ITERATIONS,
                .twt_interval = CONFIG_SENSOR_TWT_TEST_3_INTERVAL,
                .twt_wake_interval = CONFIG_SENSOR_TWT_TEST_3_SESSION_DURATION,
                .test_id = 3,
                .wake_ahead_ms = 100,
};
test_sensor_twt(&test_sem, &test_settings_3);
#endif //CONFIG_SENSOR_TWT_TEST_3

#ifdef CONFIG_SENSOR_TWT_TEST_4
struct test_sensor_twt_settings test_settings_4 = {
                .iterations = CONFIG_SENSOR_TWT_TEST_4_ITERATIONS,
                .twt_interval = CONFIG_SENSOR_TWT_TEST_4_INTERVAL,
                .twt_wake_interval = CONFIG_SENSOR_TWT_TEST_4_SESSION_DURATION,
                .test_id = 4,
                .wake_ahead_ms = 100,
};
test_sensor_twt(&test_sem, &test_settings_4);
#endif //CONFIG_SENSOR_TWT_TEST_4

#ifdef CONFIG_SENSOR_TWT_TEST_5
struct test_sensor_twt_settings test_settings_5 = {
                .iterations = CONFIG_SENSOR_TWT_TEST_5_ITERATIONS,
                .twt_interval = CONFIG_SENSOR_TWT_TEST_5_INTERVAL,
                .twt_wake_interval = CONFIG_SENSOR_TWT_TEST_5_SESSION_DURATION,
                .test_id = 5,
                .wake_ahead_ms = 100,
};
test_sensor_twt(&test_sem, &test_settings_5);
#endif //CONFIG_SENSOR_TWT_TEST_5

#endif //CONFIG_SENSOR_TWT_TESTS_ENABLE
}

#endif //CONFIG_SENSOR_TESTS_ENABLE


//large packet tests
#ifdef CONFIG_LARGE_PACKET_TESTS_ENABLE

//PS Tests
{
    #ifdef CONFIG_LARGE_PACKET_PS_TESTS_ENABLE

    #ifdef CONFIG_LARGE_PACKET_PS_TEST_1
    struct test_large_packet_ps_settings test_settings_1 = {
                    .iterations = CONFIG_LARGE_PACKET_PS_TEST_1_ITERATIONS,
                    .send_interval = CONFIG_LARGE_PACKET_PS_TEST_1_SEND_INTERVAL,
                    .test_id = 1,
                    .bytes = CONFIG_LARGE_PACKET_PS_TEST_1_NUM_BYTES,

                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_1_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_1_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_1_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_1_SERVER_ECHO

                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_1_PS_ENABLED
                    .ps_enabled = PS_MODE_ENABLED,
                    #else
                    .ps_enabled = PS_MODE_DISABLED,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_1_PS_ENABLED

                    #ifndef CONFIG_LARGE_PACKET_PS_TEST_1_MODE_WMM
                    .ps_mode = PS_MODE_LEGACY,
                    #else
                    .ps_mode = PS_MODE_WMM,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_1_MODE_WMM

                    #ifndef CONFIG_LARGE_PACKET_PS_TEST_1_WAKEUP_MODE_LISTENINTERVAL
                    .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                    #else
                    .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_1_WAKEUP_MODE_LISTENINTERVAL
    };
    test_large_packet_ps(&test_sem, &test_settings_1);
    #endif //CONFIG_LARGE_PACKET_PS_TEST_1

    #ifdef CONFIG_LARGE_PACKET_PS_TEST_2
    struct test_large_packet_ps_settings test_settings_2 = {
                    .iterations = CONFIG_LARGE_PACKET_PS_TEST_2_ITERATIONS,
                    .send_interval = CONFIG_LARGE_PACKET_PS_TEST_2_SEND_INTERVAL,
                    .test_id = 2,
                    .bytes = CONFIG_LARGE_PACKET_PS_TEST_2_NUM_BYTES,

                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_2_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_2_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_2_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_2_SERVER_ECHO

                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_2_PS_ENABLED
                    .ps_enabled = PS_MODE_ENABLED,
                    #else
                    .ps_enabled = PS_MODE_DISABLED,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_2_PS_ENABLED

                    #ifndef CONFIG_LARGE_PACKET_PS_TEST_2_MODE_WMM
                    .ps_mode = PS_MODE_LEGACY,
                    #else
                    .ps_mode = PS_MODE_WMM,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_2_MODE_WMM

                    #ifndef CONFIG_LARGE_PACKET_PS_TEST_2_WAKEUP_MODE_LISTENINTERVAL
                    .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                    #else
                    .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_2_WAKEUP_MODE_LISTENINTERVAL
    };
    test_large_packet_ps(&test_sem, &test_settings_2);
    #endif //CONFIG_LARGE_PACKET_PS_TEST_2

    #ifdef CONFIG_LARGE_PACKET_PS_TEST_3
    struct test_large_packet_ps_settings test_settings_3 = {
                    .iterations = CONFIG_LARGE_PACKET_PS_TEST_3_ITERATIONS,
                    .send_interval = CONFIG_LARGE_PACKET_PS_TEST_3_SEND_INTERVAL,
                    .test_id = 3,
                    .bytes = CONFIG_LARGE_PACKET_PS_TEST_3_NUM_BYTES,

                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_3_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_3_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,                    
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_3_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_3_SERVER_ECHO

                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_3_PS_ENABLED
                    .ps_enabled = PS_MODE_ENABLED,
                    #else
                    .ps_enabled = PS_MODE_DISABLED,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_3_PS_ENABLED

                    #ifndef CONFIG_LARGE_PACKET_PS_TEST_3_MODE_WMM
                    .ps_mode = PS_MODE_LEGACY,
                    #else
                    .ps_mode = PS_MODE_WMM,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_3_MODE_WMM

                    #ifndef CONFIG_LARGE_PACKET_PS_TEST_3_WAKEUP_MODE_LISTENINTERVAL
                    .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                    #else
                    .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_3_WAKEUP_MODE_LISTENINTERVAL
    };
    test_large_packet_ps(&test_sem, &test_settings_3);
    #endif //CONFIG_LARGE_PACKET_PS_TEST_3

    #endif //CONFIG_LARGE_PACKET_PS_TESTS_ENABLE
}

//TWT Tests
{
    #ifdef CONFIG_LARGE_PACKET_TWT_TESTS_ENABLE

    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_1
    struct test_large_packet_twt_settings test_settings_1 = {
                    .iterations = CONFIG_LARGE_PACKET_TWT_TEST_1_ITERATIONS,
                    .twt_interval = CONFIG_LARGE_PACKET_TWT_TEST_1_INTERVAL,
                    .twt_wake_interval = CONFIG_LARGE_PACKET_TWT_TEST_1_SESSION_DURATION,
                    .test_id = 1,
                    .wake_ahead_ms = 1000,
                    .bytes = CONFIG_LARGE_PACKET_TWT_TEST_1_NUM_BYTES,
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_1_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_1_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_1_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_1_SERVER_SREQ_LRES
    };
    test_large_packet_twt(&test_sem, &test_settings_1);
    #endif //CONFIG_LARGE_PACKET_TWT_TEST_1
    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_2
    struct test_large_packet_twt_settings test_settings_2 = {
                    .iterations = CONFIG_LARGE_PACKET_TWT_TEST_2_ITERATIONS,
                    .twt_interval = CONFIG_LARGE_PACKET_TWT_TEST_2_INTERVAL,
                    .twt_wake_interval = CONFIG_LARGE_PACKET_TWT_TEST_2_SESSION_DURATION,
                    .test_id = 2,
                    .wake_ahead_ms = 1000,
                    .bytes = CONFIG_LARGE_PACKET_TWT_TEST_2_NUM_BYTES,
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_2_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_2_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_2_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_2_SERVER_SREQ_LRES
    };
    test_large_packet_twt(&test_sem, &test_settings_2);
    #endif //CONFIG_LARGE_PACKET_TWT_TEST_2

    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_3
    struct test_large_packet_twt_settings test_settings_3 = {
                    .iterations = CONFIG_LARGE_PACKET_TWT_TEST_3_ITERATIONS,
                    .twt_interval = CONFIG_LARGE_PACKET_TWT_TEST_3_INTERVAL,
                    .twt_wake_interval = CONFIG_LARGE_PACKET_TWT_TEST_3_SESSION_DURATION,
                    .test_id = 3,
                    .wake_ahead_ms = 1000,
                    .bytes = CONFIG_LARGE_PACKET_TWT_TEST_3_NUM_BYTES,
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_3_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_3_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_3_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_3_SERVER_SREQ_LRES
    };
    test_large_packet_twt(&test_sem, &test_settings_3);
    #endif //CONFIG_LARGE_PACKET_TWT_TEST_3


    #endif //CONFIG_LARGE_PACKET_TWT_TESTS_ENABLE

}

#endif //CONFIG_LARGE_PACKET_TESTS_ENABLE

}