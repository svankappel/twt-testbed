/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

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

#ifdef CONFIG_COAP_SECURE
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

//set socket options for DTLS
int set_socket_dtls_options(int sock)
{
	int err;

	// set peer verification
	#ifdef CONFIG_COAP_DTLS_PEER_VERIFY
	int verify = TLS_PEER_VERIFY_REQUIRED;
	#else
	int verify = TLS_PEER_VERIFY_NONE;
	#endif

	err = setsockopt(sock, SOL_TLS, TLS_PEER_VERIFY, &verify, sizeof(verify));
	if (err) {
		LOG_ERR("Failed to setup peer verification, errno %d\n", errno);
		return -errno;
	}

	// set cipher suite
	int ciphersuite_list[] = {
		#ifndef CONFIG_COAP_DTLS_CIPHERSUITE_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
	    MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256,
		#else //CONFIG_COAP_DTLS_CIPHERSUITE_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
		MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256,
		#endif //CONFIG_COAP_DTLS_CIPHERSUITE_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
	};

	err = setsockopt(sock, SOL_TLS, TLS_CIPHERSUITE_LIST, ciphersuite_list, sizeof(ciphersuite_list));
	if (err) {
		LOG_ERR("Failed to set cipher suite, err %d", errno);
		return err;
	}


	// set credentials
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

	// set CID
	#ifdef CONFIG_COAP_DTLS_CID
	int cid = TLS_DTLS_CID_ENABLED;
	#else
	int cid = TLS_DTLS_CID_DISABLED;
	#endif
	
	err = setsockopt(sock, SOL_TLS, TLS_DTLS_CID, &cid, sizeof(int));
	if (err) {
		LOG_ERR("Failed to setup CID, errno %d\n", errno);
		return -errno;
	}

	return 0;
}
#endif //CONFIG_COAP_SECURE