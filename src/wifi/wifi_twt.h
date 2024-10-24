/**
 * @file wifi_twt.h
 * @brief Header file for TWT (Target Wake Time) Wi-Fi operations.
 *
 * This file contains the declarations of functions used to initialize,
 * set up, and tear down TWT operations in a Wi-Fi application.
 */

#ifndef WIFI_TWT_H
#define WIFI_TWT_H
#include <zephyr/kernel.h>

/**
 * @brief Initializes the TWT module.
 *
 * This function performs any necessary initialization for the TWT module.
 *
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int wifi_twt_init();

/**
 * @brief Sets up a TWT session.
 *
 * This function configures a TWT session with the specified wake interval
 * and TWT interval. This function will block until the TWT session is set up
 *
 * @param twt_wake_interval_ms The wake interval in milliseconds.
 * @param twt_interval_ms The TWT interval in milliseconds.
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int wifi_twt_setup(uint32_t twt_wake_interval_ms, uint32_t twt_interval_ms);

/**
 * @brief Tears down the TWT session.
 *
 * This function terminates the current TWT session. This function will block
 * until the TWT session is torn down.
 *
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int wifi_twt_teardown();

/**
 * @brief Registers a callback function for TWT (Target Wake Time) event notifications.
 *
 * This function allows the user to register a callback that will be invoked
 * whenever a TWT event occurs. The callback function should accept a single
 * integer parameter that indicates whether the device is awake (1) or not (0).
 *
 * @param callback A pointer to the callback function that will handle TWT events.
 *                 The callback function should have the following signature:
 *                 void callback(const int awake);
 */
void wifi_twt_register_event_callback(void (*callback)(void * user_data), uint32_t wake_ahead,void * user_data);

/**
 * @brief Check if TWT is enabled.
 *
 * This function checks if TWT is enabled.
 *
 * @return true if TWT is enabled, false otherwise.
 */
bool wifi_twt_is_enabled();

uint32_t wifi_twt_get_interval_ms(void);
uint32_t wifi_twt_get_wake_interval_ms(void);

#endif // WIFI_TWT_H
