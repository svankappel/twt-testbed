#include "test_runner.h"

#include "test_sensor_ps.h"
#include "test_sensor_twt.h"

#include "test_large_packet_ps.h"
#include "test_large_packet_twt.h"

#include "test_actuator_ps.h"
#include "test_actuator_twt.h"

static K_SEM_DEFINE(test_sem, 0, 1);



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

                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_1_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_1_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_1_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_1_SERVER_ECHO
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER


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

                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_2_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_2_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_2_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_2_SERVER_ECHO
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER

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
                    
                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_3_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_PS_TEST_3_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,                    
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_3_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_PS_TEST_3_SERVER_ECHO
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER

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
                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_1_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_1_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_1_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_1_SERVER_SREQ_LRES
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
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
                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_2_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_2_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_2_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_2_SERVER_SREQ_LRES
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
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
                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_3_SERVER_SREQ_LRES 
                    .large_packet_config = SREQ_LRES,
                    #else
                    #ifdef CONFIG_LARGE_PACKET_TWT_TEST_3_SERVER_LREQ_LRES
                    .large_packet_config = LREQ_LRES,
                    #else
                    .large_packet_config = LREQ_SRES,
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_3_SERVER_LREQ_LRES
                    #endif //CONFIG_LARGE_PACKET_TWT_TEST_3_SERVER_SREQ_LRES
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
    };
    test_large_packet_twt(&test_sem, &test_settings_3);
    #endif //CONFIG_LARGE_PACKET_TWT_TEST_3


    #endif //CONFIG_LARGE_PACKET_TWT_TESTS_ENABLE

}

#endif //CONFIG_LARGE_PACKET_TESTS_ENABLE

#ifdef CONFIG_ACTUATOR_TESTS_ENABLE
{
    #ifdef CONFIG_ACTUATOR_PS_TESTS_ENABLE

    #ifdef CONFIG_ACTUATOR_PS_TEST_1
    struct test_actuator_ps_settings test_settings_1 = {

                    .test_time_s = CONFIG_ACTUATOR_PS_TEST_1_TIME,
                    .test_id = 1,

                    #ifndef CONFIG_ACTUATOR_PS_TEST_1_MODE_WMM
                    .ps_mode = PS_MODE_LEGACY,
                    #else
                    .ps_mode = PS_MODE_WMM,
                    #endif //CONFIG_ACTUATOR_PS_TEST_1_MODE_WMM

                    #ifndef CONFIG_ACTUATOR_PS_TEST_1_WAKEUP_MODE_LISTENINTERVAL
                    .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                    #else
                    .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                    #endif //CONFIG_ACTUATOR_PS_TEST_1_WAKEUP_MODE_LISTENINTERVAL

                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    .min_interval = CONFIG_ACTUATOR_PS_TEST_1_MIN_INTERVAL,
                    .max_interval = CONFIG_ACTUATOR_PS_TEST_1_MAX_INTERVAL,
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
    };
    test_actuator_ps(&test_sem, &test_settings_1);
    #endif //CONFIG_ACTUATOR_PS_TEST_1

    #endif //CONFIG_ACTUATOR_PS_TESTS_ENABLE
}

{
#ifdef CONFIG_ACTUATOR_TWT_TESTS_ENABLE

#ifdef CONFIG_ACTUATOR_TWT_TEST_1
struct test_actuator_twt_settings test_settings_1 = {
                .test_time_s = CONFIG_ACTUATOR_TWT_TEST_1_TIME,
                .twt_interval = CONFIG_ACTUATOR_TWT_TEST_1_INTERVAL,
                .twt_wake_interval = CONFIG_ACTUATOR_TWT_TEST_1_SESSION_DURATION,
                .test_id = 1,

                #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                .min_interval = CONFIG_ACTUATOR_TWT_TEST_1_MIN_INTERVAL,
                .max_interval = CONFIG_ACTUATOR_TWT_TEST_1_MAX_INTERVAL,
                #endif //CONFIG_COAP_TWT_TESTBED_SERVER
};
test_actuator_twt(&test_sem, &test_settings_1);
#endif //CONFIG_ACTUATOR_TWT_TEST_1

#endif //CONFIG_ACTUATOR_TWT_TESTS_ENABLE
}



#endif //CONFIG_ACTUATOR_TESTS_ENABLE

}