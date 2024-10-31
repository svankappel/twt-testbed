/**
 * @file wifi_sta.h
 * @brief Header file for WiFi station initialization.
 *
 * This file contains the declaration for the WiFi station initialization function.
 */


#ifndef WIFI_STA_H
#define WIFI_STA_H

#include <zephyr/kernel.h>

 /**
 * @brief Initializes the WiFi station.
 *
 * This function sets up the WiFi station mode and connects to the network specified
 * in the configuration.
 *
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int wifi_init(void);

/**
 * @brief Initiate a WiFi connection. This function will block until the    
 * connection is complete.
 *
 * @return 0 on success, negative error code on failure
 */
int wifi_connect(void);

/**
 * @brief Initiate a WiFi disconnection. This function will block until the
 * disconnection is complete.
 *
 * @return 0 on success, negative error code on failure
 */
int wifi_disconnect(void);

/**
 * @brief Register a callback function to be called when the WiFi gets disconnected.
 *
 * @param cb The callback function to be registered.
 */
void wifi_register_disconnected_cb(void (*cb)(void));

#endif /* WIFI_STA_H */