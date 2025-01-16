/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** 
 * @file coap.h
 * @brief Header file for CoAP utils
 *
 * This file contains the declarations of utility functions used by the CoAP client.
 */

#ifndef COAP_UTILS_H
#define COAP_UTILS_H

#include <zephyr/kernel.h>
#include <zephyr/net/coap.h>
#include <zephyr/net/socket.h>

#define TOKEN_LEN 8

#ifndef CONFIG_IP_PROTO_IPV6
/**
 * @brief Resolve the server address for IPv4.
 *
 * @param server_ptr Pointer to the sockaddr_in structure to be filled with the server address.
 * @return int 0 on success, negative error code on failure.
 */
int server_resolve(struct sockaddr_in* server_ptr);
#else // CONFIG_IP_PROTO_IPV6
/**
 * @brief Resolve the server address for IPv6.
 *
 * @param server_ptr Pointer to the sockaddr_in6 structure to be filled with the server address.
 * @return int 0 on success, negative error code on failure.
 */
int server_resolve(struct sockaddr_in6* server_ptr);
#endif // CONFIG_IP_PROTO_IPV6

/**
 * @brief Generate a random token.
 *
 * @param token Pointer to the buffer where the token will be stored.
 */
void random_token(uint8_t *token);

/**
 * @brief Initialize the pool of pending requests.
 *
 * @param max_timeout Maximum timeout value for pending requests.
 */
void init_pending_request_pool(uint32_t max_timeout);

/**
 * @brief Add a pending request to the pool.
 *
 * @param token Pointer to the token of the request to be added.
 * @return int 0 on success, negative error code on failure.
 */
int add_pending_request(uint8_t * token);

/**
 * @brief Remove a pending request from the pool.
 *
 * @param token Pointer to the token of the request to be removed.
 * @return uint32_t Timeout value of the removed request.
 */
uint32_t remove_pending_request(uint8_t * token);

#endif // COAP_UTILS_H

