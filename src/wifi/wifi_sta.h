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
 * @brief Initiate a WiFi connection
 *
 * @return 0 on success, negative error code on failure
 */
int wifi_connect(void);

/**
 * @brief Initiate a WiFi disconnection
 *
 * @return 0 on success, negative error code on failure
 */
int wifi_disconnect(void);


/**
 * @brief Sets up TWT parameters.
 *
 * This function configures the TWT wake interval and TWT interval.
 *
 * @param twt_wake_interval_ms Wake interval in milliseconds.
 * @param twt_interval_ms TWT interval in milliseconds.
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int wifi_twt_setup(uint32_t twt_wake_interval_ms, uint32_t twt_interval_ms);

/**
 * @brief Tears down the TWT setup.
 *
 * This function disables the TWT configuration.
 *
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int wifi_twt_teardown(void);
/**
 * @brief Set Wi-Fi power save mode to legacy.
 *
 * This function enables the legacy power save mode for Wi-Fi.
 *
 * @return 0 on success, -1 on failure.
 */
int wifi_ps_mode_legacy(void);

/**
 * @brief Set Wi-Fi power save mode to WMM.
 *
 * This function enables the WMM power save mode for Wi-Fi.
 *
 * @return 0 on success, -1 on failure.
 */
int wifi_ps_mode_wmm(void);

/**
 * @brief Set Wi-Fi wakeup mode to DTIM.
 *
 * This function sets the Wi-Fi wakeup mode to DTIM.
 *
 * @return 0 on success, -1 on failure.
 */
int wifi_ps_wakeup_dtim(void);

/**
 * @brief Set Wi-Fi wakeup mode to listen interval.
 *
 * This function sets the Wi-Fi wakeup mode to listen interval.
 *
 * @return 0 on success, -1 on failure.
 */
int wifi_ps_wakeup_listen_interval(void);

/**
 * @brief Enable Wi-Fi power save mode.
 *
 * This function enables the Wi-Fi power save mode.
 *
 * @return 0 on success, -1 on failure.
 */
int wifi_ps_enable(void);

/**
 * @brief Disable Wi-Fi power save mode.
 *
 * This function disables the Wi-Fi power save mode.
 *
 * @return 0 on success, -1 on failure.
 */
int wifi_ps_disable(void);

/**
 * @brief Set Wi-Fi listen interval.
 *
 * This function sets the listen interval for Wi-Fi power save mode.
 *
 * @param interval The listen interval to set.
 * @return 0 on success, -1 on failure.
 */
int wifi_ps_set_listen_interval(int interval);

#endif // WIFI_STA_H