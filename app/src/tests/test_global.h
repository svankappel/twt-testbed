#ifndef TEST_GLOBAL_H
#define TEST_GLOBAL_H

#define TESTBED_SENSOR_RESOURCE "sensor"
#define TESTBED_ACTUATOR_RESOURCE "observe"
#define TESTBED_VALIDATE_RESOURCE "validate"
#define TESTBED_STAT_RESOURCE "stat"
#define TESTBED_LARGE_UPLOAD_RESOURCE "largeuploadack"
#define TESTBED_LARGE_DOWNLOAD_RESOURCE "largedownload"
#define TESTBED_LARGE_UPLOAD_DOWNLOAD_RESOURCE "largeuploadecho"

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


#endif //TEST_GLOBAL_H