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
int twt_init();

/**
 * @brief Sets up a TWT session.
 *
 * This function configures a TWT session with the specified wake interval
 * and TWT interval.
 *
 * @param twt_wake_interval_ms The wake interval in milliseconds.
 * @param twt_interval_ms The TWT interval in milliseconds.
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int twt_setup(uint32_t twt_wake_interval_ms, uint32_t twt_interval_ms);

/**
 * @brief Tears down the TWT session.
 *
 * This function terminates the current TWT session.
 *
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int twt_teardown();

#endif // WIFI_TWT_H