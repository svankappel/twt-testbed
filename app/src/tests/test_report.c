#include "test_report.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

LOG_MODULE_REGISTER(test_report, CONFIG_MY_TEST_LOG_LEVEL);

static char testbed_setup[512] = {0};

static void generate_testbed_setup()
{
    #ifdef CONFIG_IP_PROTO_IPV6
        char ip[] = "IPv6";
    #else
        char ip[] = "IPv4";
    #endif //CONFIG_IP_PROTO

    #ifdef CONFIG_TWT_IMPLICIT
        char implicit[] = "true";
    #else
        char implicit[] = "false";
    #endif //CONFIG_TWT_IMPLICIT

    #ifdef CONFIG_TWT_ANNOUNCED_MODE
        char announced[] = "true";
    #else
        char announced[] = "false";
    #endif //CONFIG_TWT_ANNOUNCED_MODE

    #ifdef CONFIG_TWT_TRIGGER
        char trigger[] = "true";
    #else
        char trigger[] = "false";
    #endif //CONFIG_TWT_TRIGGER


    #ifdef CONFIG_COAP_SECURE
        #ifdef CONFIG_COAP_DTLS_PEER_VERIFY
        char peer[] = "Enabled";
        #else
            char peer[] = "Disabled";
        #endif //CONFIG_COAP_DTLS_PEER_VERIFY

        #ifdef CONFIG_COAP_DTLS_CID
            char cid[] = "Enabled";
        #else
            char cid[] = "Disabled";
        #endif //CONFIG_COAP_DTLS_CID

        #ifdef CONFIG_COAP_DTLS_CIPHERSUITE_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
            char ciphersuite[] = "TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256";
        #else
            char ciphersuite[] = "TLS_PSK_WITH_AES_128_CBC_SHA256";
        #endif //CONFIG_COAP_DTLS_CIPHERSUITE

        sprintf(testbed_setup,
             "\"testbed_setup\": {\n"
             "    \"CoAP_Server\": \"%s\",\n"
             "    \"DTLS\": \"%s\",\n"
             "    \"DTLS_Peer_Verification\": \"%s\",\n"
             "    \"DTLS_Connection_ID\": \"%s\",\n"
             "    \"DTLS_Ciphersuite\": \"%s\",\n"
             "    \"IP_Protocol\": \"%s\",\n"
             "    \"Wi-Fi_TWT_Implicit\": %s,\n"
             "    \"Wi-Fi_TWT_Announced\": %s,\n"
             "    \"Wi-Fi_TWT_Trigger\": %s,\n"
             "    \"Wi-Fi_PS_Listen_Interval\": %d\n"
             "}",
             CONFIG_COAP_TEST_SERVER_HOSTNAME,
             "Enabled",
             peer,
             cid,
             ciphersuite,
             ip,
             implicit,
             announced,
             trigger,
             CONFIG_PS_LISTEN_INTERVAL);


    #else

        snprintf(testbed_setup, sizeof(testbed_setup),
                "\"testbed_setup\": {\n"
                "    \"CoAP_Server\": \"%s\",\n"
                "    \"DTLS\": \"%s\",\n"
                "    \"IP_Protocol\": \"%s\",\n"
                "    \"Wi-Fi_TWT_Implicit\": %s,\n"
                "    \"Wi-Fi_TWT_Announced\": %s,\n"
                "    \"Wi-Fi_TWT_Trigger\": %s,\n"
                "    \"Wi-Fi_PS_Listen_Interval\": %d\n"
                "}",
                CONFIG_COAP_TEST_SERVER_HOSTNAME,
                "Disabled",
                ip,
                implicit,
                announced,
                trigger,
                CONFIG_PS_LISTEN_INTERVAL);

    #endif //CONFIG_COAP_SECURE



}


void test_report_print(struct test_report *report)
{
    generate_testbed_setup();

    printk("<<<<<<< Test Report Start >>>>>>\n");
    printk("{");
    printk("\n%s", report->test_title);
    printk(",\n%s", testbed_setup);
    printk(",\n%s", report->test_setup);
    printk(",\n%s", report->results);
    if(report->latency_histogram[0] != '\0'){
        //break the buffer in chunks because it is too big to printed in one printk
        for (int i = 0; i < 8; i++) {
            int offset = i * 512;
            char buf[512];
            memcpy(buf, &report->latency_histogram[offset], 512);
            // Print each chunk
            printk("%.*s", 512, buf);
        }
    }
    printk("\n}");
    printk("\n<<<<<<< Test Report End >>>>>>\n");
}