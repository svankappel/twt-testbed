#include "coap_security.h"

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/random/random.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/coap.h>
#include <mbedtls/ssl_ciphersuites.h>
#include <zephyr/net/tls_credentials.h>

LOG_MODULE_REGISTER(coap_security, CONFIG_MY_COAP_LOG_LEVEL);

//security tag for credentials
#define SEC_TAG 12


int configure_psk(void)
{
    int err;

    /* Add the PSK identity */
    err = tls_credential_add(SEC_TAG, TLS_CREDENTIAL_PSK_ID, CONFIG_COAP_SERVER_DTLS_PSK_ID, strlen(CONFIG_COAP_SERVER_DTLS_PSK_ID));
    if (err < 0) {
        LOG_ERR("Failed to add PSK identity: %d", err);
        return err;
    }

    /* Add the PSK */
    err = tls_credential_add(SEC_TAG, TLS_CREDENTIAL_PSK, CONFIG_COAP_SERVER_DTLS_PSK, strlen(CONFIG_COAP_SERVER_DTLS_PSK));
    if (err < 0) {
        LOG_ERR("Failed to add PSK: %d", err);
        return err;
    }

    LOG_INF("PSK and identity successfully configured.");
    return 0;
}

/**@brief Initialize the CoAP client */
int set_socket_dtls_options(int sock)
{
	int err;

	int ciphersuite_list[] = {
	//  MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8,
	//  MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256,
	//  MBEDTLS_TLS_PSK_WITH_AES_128_CCM,
		MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256,
	//  MBEDTLS_TLS_PSK_WITH_AES_256_GCM_SHA384,
	//  MBEDTLS_TLS_PSK_WITH_AES_256_CCM,
	//  MBEDTLS_TLS_PSK_WITH_AES_256_CCM_8
	};


	err = setsockopt(sock, SOL_TLS, TLS_CIPHERSUITE_LIST, ciphersuite_list, sizeof(ciphersuite_list));
	if (err) {
		LOG_ERR("Failed to set cipher suite, err %d", errno);
		return err;
	}


	sec_tag_t sec_tag_list[] = { SEC_TAG };
	err = setsockopt(sock, SOL_TLS, TLS_SEC_TAG_LIST, sec_tag_list,sizeof(sec_tag_t) * ARRAY_SIZE(sec_tag_list));
	if (err) {
		LOG_ERR("Failed to setup socket security tag, errno %d\n", errno);
		return -errno;
	}


	err = setsockopt(sock, SOL_TLS, TLS_HOSTNAME, CONFIG_COAP_TEST_SERVER_HOSTNAME,strlen(CONFIG_COAP_TEST_SERVER_HOSTNAME));
	if (err) {
		LOG_ERR("Failed to setup TLS hostname (%s), errno %d\n",
		CONFIG_COAP_TEST_SERVER_HOSTNAME, errno);
		return -errno;
	}

	return 0;
}
