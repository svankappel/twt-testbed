/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file coap_security.h
 * @brief Header file for CoAP security configurations.
 *
 * This file contains function declarations for configuring Pre-Shared Key (PSK)
 * and setting DTLS options on a socket for secure CoAP communication.
 */
#ifndef COAP_SECURITY_H
#define COAP_SECURITY_H

#ifdef CONFIG_COAP_SECURE

/**
 * @brief Configures the Pre-Shared Key (PSK) for secure CoAP communication.
 *
 * This function sets up the necessary PSK for DTLS communication in CoAP.
 *
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int configure_psk(void);

/**
 * @brief Sets DTLS options on a given socket.
 *
 * This function configures the necessary DTLS options on the specified socket
 * to enable secure CoAP communication.
 *
 * @param sock The socket on which to set DTLS options.
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int set_socket_dtls_options(int sock);

#endif // CONFIG_COAP_SECURE

#endif //COAP_SECURITY_H