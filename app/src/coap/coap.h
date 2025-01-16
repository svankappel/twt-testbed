/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** 
 * @file coap.h
 * @brief Header file for CoAP (Constrained Application Protocol) functionalities.
 *
 * This file contains the declarations for initializing and managing a CoAP client.
 * The CoAP client is specifically designed to work with the TWT testbed.
 */

#ifndef COAP_H
#define COAP_H

#include <zephyr/kernel.h>

/**
 * @brief Initialize the CoAP client.
 *
 * @return 0 on success, negative error code on failure.
 */
int coap_init();

/**
 * @brief Initialize the CoAP request pool with a specified timeout.
 *
 * @param requests_timeout Timeout value for CoAP requests.
 */
void coap_init_pool(uint32_t requests_timeout);

#ifdef CONFIG_COAP_TWT_TESTBED_SERVER
/**
 * @brief Validate the CoAP configuration.
 *
 * @return 0 on success, negative error code on failure.
 */
int coap_validate();

/**
 * @brief Get the CoAP statistics.
 *
 * @return server counter value on success, negative error code on failure.
 */
int coap_get_stat();

/**
 * @brief Get the actuator test statistics.
 *
 * @param buffer Buffer to store the actuator statistic string.
 * @return 0 on success, negative error code on failure.
 */
int coap_get_actuator_stat(char * buffer);
#endif //CONFIG_COAP_TWT_TESTBED_SERVER

/**
 * @brief Send a CoAP PUT request.
 *
 * @param resource The resource to be updated.
 * @param payload The payload to be sent.
 * @return 0 on success, negative error code on failure.
 */
int coap_put(char *resource, uint8_t *payload);

/**
 * @brief Send a CoAP GET request with observe option.
 *
 * @param resource The resource to be observed.
 * @param payload The payload to be sent.
 * @return 0 on success, negative error code on failure.
 */
int coap_observe(char *resource, uint8_t *payload);

/**
 * @brief Cancel the CoAP observe request.
 *
 * @return 0 on success, negative error code on failure.
 */
int coap_cancel_observe();

/**
 * @brief Enable emergency transmission
 */
void coap_emergency_enable();

/**
 * @brief Disable emergency transmission
 */
void coap_emergency_disable();

/**
 * @brief Register a callback function for CoAP PUT response.
 *
 * @param callback The callback function to be registered.
 */
void coap_register_put_response_callback(void (*callback)(uint32_t time, uint8_t * payload, uint16_t payload_len));

/**
 * @brief Register a callback function for CoAP observe response (notification).
 *
 * @param callback The callback function to be registered.
 */
void coap_register_obs_response_callback(void (*callback)(uint8_t * payload, uint16_t payload_len));

#endif // COAP_H