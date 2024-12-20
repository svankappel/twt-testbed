#include "test_runner.h"

#include "test_sensor_ps.h"
#include "test_sensor_twt.h"

#include "test_large_packet_ps.h"
#include "test_large_packet_twt.h"

#include "test_actuator_ps.h"
#include "test_actuator_twt.h"

#include "test_multi_packet_ps.h"
#include "test_multi_packet_twt.h"

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

                #ifdef CONFIG_SENSOR_TWT_TEST_1_RECOVER
                .recover = true,
                .recover_max_pending = CONFIG_SENSOR_TWT_TEST_1_RECOVER_MAX_PENDING,
                #else
                .recover = false,
                #endif //CONFIG_SENSOR_TWT_TEST_1_RECOVER
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
                
                #ifdef CONFIG_SENSOR_TWT_TEST_2_RECOVER
                .recover = true,
                .recover_max_pending = CONFIG_SENSOR_TWT_TEST_2_RECOVER_MAX_PENDING,
                #else
                .recover = false,
                #endif //CONFIG_SENSOR_TWT_TEST_2_RECOVER
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
                
                #ifdef CONFIG_SENSOR_TWT_TEST_3_RECOVER
                .recover = true,
                .recover_max_pending = CONFIG_SENSOR_TWT_TEST_3_RECOVER_MAX_PENDING,
                #else
                .recover = false,
                #endif //CONFIG_SENSOR_TWT_TEST_3_RECOVER
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
                
                #ifdef CONFIG_SENSOR_TWT_TEST_4_RECOVER
                .recover = true,
                .recover_max_pending = CONFIG_SENSOR_TWT_TEST_4_RECOVER_MAX_PENDING,
                #else
                .recover = false,
                #endif //CONFIG_SENSOR_TWT_TEST_4_RECOVER
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

                    #ifdef CONFIG_ACTUATOR_PS_TEST_1_ECHO
                    .echo = true,
                    #else
                    .echo = false,
                    #endif //CONFIG_ACTUATOR_PS_TEST_1_ECHO
                    
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
    };
    test_actuator_ps(&test_sem, &test_settings_1);
    #endif //CONFIG_ACTUATOR_PS_TEST_1

    #ifdef CONFIG_ACTUATOR_PS_TEST_2
    struct test_actuator_ps_settings test_settings_2 = {

                    .test_time_s = CONFIG_ACTUATOR_PS_TEST_2_TIME,
                    .test_id = 2,

                    #ifndef CONFIG_ACTUATOR_PS_TEST_2_MODE_WMM
                    .ps_mode = PS_MODE_LEGACY,
                    #else
                    .ps_mode = PS_MODE_WMM,
                    #endif //CONFIG_ACTUATOR_PS_TEST_2_MODE_WMM

                    #ifndef CONFIG_ACTUATOR_PS_TEST_2_WAKEUP_MODE_LISTENINTERVAL
                    .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                    #else
                    .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                    #endif //CONFIG_ACTUATOR_PS_TEST_2_WAKEUP_MODE_LISTENINTERVAL

                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    .min_interval = CONFIG_ACTUATOR_PS_TEST_2_MIN_INTERVAL,
                    .max_interval = CONFIG_ACTUATOR_PS_TEST_2_MAX_INTERVAL,

                    #ifdef CONFIG_ACTUATOR_PS_TEST_2_ECHO
                    .echo = true,
                    #else
                    .echo = false,
                    #endif //CONFIG_ACTUATOR_PS_TEST_2_ECHO
                    
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
    };
    test_actuator_ps(&test_sem, &test_settings_2);
    #endif //CONFIG_ACTUATOR_PS_TEST_2

    #ifdef CONFIG_ACTUATOR_PS_TEST_3
    struct test_actuator_ps_settings test_settings_3 = {

                    .test_time_s = CONFIG_ACTUATOR_PS_TEST_3_TIME,
                    .test_id = 3,

                    #ifndef CONFIG_ACTUATOR_PS_TEST_3_MODE_WMM
                    .ps_mode = PS_MODE_LEGACY,
                    #else
                    .ps_mode = PS_MODE_WMM,
                    #endif //CONFIG_ACTUATOR_PS_TEST_3_MODE_WMM

                    #ifndef CONFIG_ACTUATOR_PS_TEST_3_WAKEUP_MODE_LISTENINTERVAL
                    .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                    #else
                    .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                    #endif //CONFIG_ACTUATOR_PS_TEST_3_WAKEUP_MODE_LISTENINTERVAL

                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    .min_interval = CONFIG_ACTUATOR_PS_TEST_3_MIN_INTERVAL,
                    .max_interval = CONFIG_ACTUATOR_PS_TEST_3_MAX_INTERVAL,

                    #ifdef CONFIG_ACTUATOR_PS_TEST_3_ECHO
                    .echo = true,
                    #else
                    .echo = false,
                    #endif //CONFIG_ACTUATOR_PS_TEST_3_ECHO
                    
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
    };
    test_actuator_ps(&test_sem, &test_settings_3);
    #endif //CONFIG_ACTUATOR_PS_TEST_3

    #ifdef CONFIG_ACTUATOR_PS_TEST_4
    struct test_actuator_ps_settings test_settings_4 = {

                    .test_time_s = CONFIG_ACTUATOR_PS_TEST_4_TIME,
                    .test_id = 4,

                    #ifndef CONFIG_ACTUATOR_PS_TEST_4_MODE_WMM
                    .ps_mode = PS_MODE_LEGACY,
                    #else
                    .ps_mode = PS_MODE_WMM,
                    #endif //CONFIG_ACTUATOR_PS_TEST_4_MODE_WMM

                    #ifndef CONFIG_ACTUATOR_PS_TEST_4_WAKEUP_MODE_LISTENINTERVAL
                    .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                    #else
                    .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                    #endif //CONFIG_ACTUATOR_PS_TEST_4_WAKEUP_MODE_LISTENINTERVAL

                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    .min_interval = CONFIG_ACTUATOR_PS_TEST_4_MIN_INTERVAL,
                    .max_interval = CONFIG_ACTUATOR_PS_TEST_4_MAX_INTERVAL,

                    #ifdef CONFIG_ACTUATOR_PS_TEST_4_ECHO
                    .echo = true,
                    #else
                    .echo = false,
                    #endif //CONFIG_ACTUATOR_PS_TEST_4_ECHO
                    
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER
    };
    test_actuator_ps(&test_sem, &test_settings_4);
    #endif //CONFIG_ACTUATOR_PS_TEST_4

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

                        #ifdef CONFIG_ACTUATOR_TWT_TEST_1_ECHO
                        .echo = true,
                        #else
                        .echo = false,
                        #endif //CONFIG_ACTUATOR_TWT_TEST_1_ECHO
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER

                    #ifdef CONFIG_ACTUATOR_TWT_TEST_1_EMERGENCY_UPLINK
                    .emergency_uplink = true,
                    #else
                    .emergency_uplink = false,
                    #endif //CONFIG_ACTUATOR_TWT_TEST_1_EMERGENCY_UPLINK
    };
    test_actuator_twt(&test_sem, &test_settings_1);
    #endif //CONFIG_ACTUATOR_TWT_TEST_1

    #ifdef CONFIG_ACTUATOR_TWT_TEST_2
    struct test_actuator_twt_settings test_settings_2 = {
                    .test_time_s = CONFIG_ACTUATOR_TWT_TEST_2_TIME,
                    .twt_interval = CONFIG_ACTUATOR_TWT_TEST_2_INTERVAL,
                    .twt_wake_interval = CONFIG_ACTUATOR_TWT_TEST_2_SESSION_DURATION,
                    .test_id = 2,

                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    .min_interval = CONFIG_ACTUATOR_TWT_TEST_2_MIN_INTERVAL,
                    .max_interval = CONFIG_ACTUATOR_TWT_TEST_2_MAX_INTERVAL,

                        #ifdef CONFIG_ACTUATOR_TWT_TEST_2_ECHO
                        .echo = true,
                        #else
                        .echo = false,
                        #endif //CONFIG_ACTUATOR_TWT_TEST_2_ECHO
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER

                    #ifdef CONFIG_ACTUATOR_TWT_TEST_2_EMERGENCY_UPLINK
                    .emergency_uplink = true,
                    #else
                    .emergency_uplink = false,
                    #endif //CONFIG_ACTUATOR_TWT_TEST_2_EMERGENCY_UPLINK
    };
    test_actuator_twt(&test_sem, &test_settings_2);
    #endif //CONFIG_ACTUATOR_TWT_TEST_2

    #ifdef CONFIG_ACTUATOR_TWT_TEST_3
    struct test_actuator_twt_settings test_settings_3 = {
                    .test_time_s = CONFIG_ACTUATOR_TWT_TEST_3_TIME,
                    .twt_interval = CONFIG_ACTUATOR_TWT_TEST_3_INTERVAL,
                    .twt_wake_interval = CONFIG_ACTUATOR_TWT_TEST_3_SESSION_DURATION,
                    .test_id = 3,

                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    .min_interval = CONFIG_ACTUATOR_TWT_TEST_3_MIN_INTERVAL,
                    .max_interval = CONFIG_ACTUATOR_TWT_TEST_3_MAX_INTERVAL,

                        #ifdef CONFIG_ACTUATOR_TWT_TEST_3_ECHO
                        .echo = true,
                        #else
                        .echo = false,
                        #endif //CONFIG_ACTUATOR_TWT_TEST_3_ECHO
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER

                    #ifdef CONFIG_ACTUATOR_TWT_TEST_3_EMERGENCY_UPLINK
                    .emergency_uplink = true,
                    #else
                    .emergency_uplink = false,
                    #endif //CONFIG_ACTUATOR_TWT_TEST_3_EMERGENCY_UPLINK
    };
    test_actuator_twt(&test_sem, &test_settings_3);
    #endif //CONFIG_ACTUATOR_TWT_TEST_3

    #ifdef CONFIG_ACTUATOR_TWT_TEST_4
    struct test_actuator_twt_settings test_settings_4 = {
                    .test_time_s = CONFIG_ACTUATOR_TWT_TEST_4_TIME,
                    .twt_interval = CONFIG_ACTUATOR_TWT_TEST_4_INTERVAL,
                    .twt_wake_interval = CONFIG_ACTUATOR_TWT_TEST_4_SESSION_DURATION,
                    .test_id = 4,

                    #ifdef CONFIG_COAP_TWT_TESTBED_SERVER
                    .min_interval = CONFIG_ACTUATOR_TWT_TEST_4_MIN_INTERVAL,
                    .max_interval = CONFIG_ACTUATOR_TWT_TEST_4_MAX_INTERVAL,

                        #ifdef CONFIG_ACTUATOR_TWT_TEST_4_ECHO
                        .echo = true,
                        #else
                        .echo = false,
                        #endif //CONFIG_ACTUATOR_TWT_TEST_4_ECHO
                    #endif //CONFIG_COAP_TWT_TESTBED_SERVER

                    #ifdef CONFIG_ACTUATOR_TWT_TEST_4_EMERGENCY_UPLINK
                    .emergency_uplink = true,
                    #else
                    .emergency_uplink = false,
                    #endif //CONFIG_ACTUATOR_TWT_TEST_4_EMERGENCY_UPLINK
    };
    test_actuator_twt(&test_sem, &test_settings_4);
    #endif //CONFIG_ACTUATOR_TWT_TEST_4

    #endif //CONFIG_ACTUATOR_TWT_TESTS_ENABLE
}

#endif //CONFIG_ACTUATOR_TESTS_ENABLE


//multi packet tests

#ifdef CONFIG_MULTI_PACKET_TESTS_ENABLE

{
    #ifdef CONFIG_MULTI_PACKET_PS_TESTS_ENABLE

    #ifdef CONFIG_MULTI_PACKET_PS_TEST_1
    struct test_multi_packet_ps_settings test_settings_1 = {

                    .iterations = CONFIG_MULTI_PACKET_PS_TEST_1_ITERATIONS,
                    .packet_number = CONFIG_MULTI_PACKET_PS_TEST_1_PACKET_NBR,
                    .test_id = 1,

                    #ifndef CONFIG_MULTI_PACKET_PS_TEST_1_MODE_WMM
                    .ps_mode = PS_MODE_LEGACY,
                    #else
                    .ps_mode = PS_MODE_WMM,
                    #endif //CONFIG_MULTI_PACKET_PS_TEST_1_MODE_WMM

                    #ifndef CONFIG_MULTI_PACKET_PS_TEST_1_WAKEUP_MODE_LISTENINTERVAL
                    .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                    #else
                    .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                    #endif //CONFIG_MULTI_PACKET_PS_TEST_1_WAKEUP_MODE_LISTENINTERVAL
    };
    test_multi_packet_ps(&test_sem, &test_settings_1);
    #endif //CONFIG_MULTI_PACKET_PS_TEST_1

    #ifdef CONFIG_MULTI_PACKET_PS_TEST_2
    struct test_multi_packet_ps_settings test_settings_2 = {

                    .iterations = CONFIG_MULTI_PACKET_PS_TEST_2_ITERATIONS,
                    .packet_number = CONFIG_MULTI_PACKET_PS_TEST_2_PACKET_NBR,
                    .test_id = 2,

                    #ifndef CONFIG_MULTI_PACKET_PS_TEST_2_MODE_WMM
                    .ps_mode = PS_MODE_LEGACY,
                    #else
                    .ps_mode = PS_MODE_WMM,
                    #endif //CONFIG_MULTI_PACKET_PS_TEST_2_MODE_WMM

                    #ifndef CONFIG_MULTI_PACKET_PS_TEST_2_WAKEUP_MODE_LISTENINTERVAL
                    .ps_wakeup_mode = PS_WAKEUP_MODE_DTIM,
                    #else
                    .ps_wakeup_mode = PS_WAKEUP_MODE_LISTEN_INTERVAL,
                    #endif //CONFIG_MULTI_PACKET_PS_TEST_2_WAKEUP_MODE_LISTENINTERVAL
    };
    test_multi_packet_ps(&test_sem, &test_settings_2);
    #endif //CONFIG_MULTI_PACKET_PS_TEST_2

    #endif //CONFIG_MULTI_PACKET_PS_TESTS_ENABLE
}

{
    #ifdef CONFIG_MULTI_PACKET_TWT_TESTS_ENABLE

    #ifdef CONFIG_MULTI_PACKET_TWT_TEST_1
    struct test_multi_packet_twt_settings test_settings_1 = {
                    .iterations = CONFIG_MULTI_PACKET_TWT_TEST_1_ITERATIONS,
                    .packet_number = CONFIG_MULTI_PACKET_TWT_TEST_1_PACKET_NBR,
                    .twt_interval = CONFIG_MULTI_PACKET_TWT_TEST_1_INTERVAL,
                    .twt_wake_interval = CONFIG_MULTI_PACKET_TWT_TEST_1_SESSION_DURATION,
                    .test_id = 1,
                    .wake_ahead_ms = 500,                 
    };
    test_multi_packet_twt(&test_sem, &test_settings_1);
    #endif //CONFIG_MULTI_PACKET_TWT_TEST_1

    #ifdef CONFIG_MULTI_PACKET_TWT_TEST_2
    struct test_multi_packet_twt_settings test_settings_2 = {
                    .iterations = CONFIG_MULTI_PACKET_TWT_TEST_2_ITERATIONS,
                    .packet_number = CONFIG_MULTI_PACKET_TWT_TEST_2_PACKET_NBR,
                    .twt_interval = CONFIG_MULTI_PACKET_TWT_TEST_2_INTERVAL,
                    .twt_wake_interval = CONFIG_MULTI_PACKET_TWT_TEST_2_SESSION_DURATION,
                    .test_id = 2,
                    .wake_ahead_ms = 500,                 
    };
    test_multi_packet_twt(&test_sem, &test_settings_2);
    #endif //CONFIG_MULTI_PACKET_TWT_TEST_2

    #endif //CONFIG_MULTI_PACKET_TWT_TESTS_ENABLE
}

#endif //CONFIG_MULTI_PACKET_TESTS_ENABLE


}