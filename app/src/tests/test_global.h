#ifndef TEST_GLOBAL_H
#define TEST_GLOBAL_H

#include <zephyr/kernel.h>

#ifdef CONFIG_COAP_TWT_TESTBED_SERVER

#define TESTBED_SENSOR_RESOURCE "sensor"
#define TESTBED_ACTUATOR_RESOURCE "actuator"
#define TESTBED_ACTUATOR_ECHO_RESOURCE "actuatorecho"
#define TESTBED_ACTUATOR_STAT_RESOURCE "actuatorstat"
#define TESTBED_VALIDATE_RESOURCE "validate"
#define TESTBED_STAT_RESOURCE "stat"
#define TESTBED_LARGE_UPLOAD_RESOURCE "largeuploadack"
#define TESTBED_LARGE_DOWNLOAD_RESOURCE "largedownload"
#define TESTBED_LARGE_UPLOAD_DOWNLOAD_RESOURCE "largeuploadecho"

#endif //CONFIG_COAP_TWT_TESTBED_SERVER

#define PS_MODE_DISABLED 0
#define PS_MODE_ENABLED 1

#define PS_MODE_LEGACY 0
#define PS_MODE_WMM 1

#define PS_WAKEUP_MODE_DTIM 0
#define PS_WAKEUP_MODE_LISTEN_INTERVAL 1


enum large_packet_config {
    LREQ_SRES,
    SREQ_LRES,
    LREQ_LRES
};

//#define STACK_SIZE 8192
//#define PRIORITY -2         //non preemptive priority
extern struct z_thread_stack_element thread_stack_[8192];



#endif //TEST_GLOBAL_H