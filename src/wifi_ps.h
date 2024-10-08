/**
 * @file wifi_ps.h
 * @brief Header file for WiFi power save modes.
 *
 * This file contains the declarations of functions to enable and disable
 * various WiFi power save modes, including legacy DTIM, WMM DTIM, legacy
 * listen interval, and WMM listen interval power save modes.
 */

#ifndef WIFI_PS_H
#define WIFI_PS_H

/**
 * @brief Enable legacy DTIM power save mode.
 *
 * This function enables the legacy DTIM (Delivery Traffic Indication Message)
 * power save mode.
 *
 * @return int Status of the operation.
 */
int wifi_ps_legacy_dtim(void);

/**
 * @brief Enable WMM DTIM power save mode.
 *
 * This function enables the WMM (Wi-Fi Multimedia) DTIM power save mode.
 *
 * @return int Status of the operation.
 */
int wifi_ps_wmm_dtim(void);

/**
 * @brief Enable legacy listen interval power save mode.
 *
 * This function enables the legacy listen interval power save mode.
 *
 * @return int Status of the operation.
 */
int wifi_ps_legacy_listen_interval(void);

/**
 * @brief Enable WMM listen interval power save mode.
 *
 * This function enables the WMM listen interval power save mode.
 *
 * @return int Status of the operation.
 */
int wifi_ps_wmm_listen_interval(void);

/**
 * @brief Set listen interval for power save mode.
 *
 * This function sets the listen interval for power save mode.
 *
 * @param interval The listen interval to be set.
 * @return int Status of the operation.
 */
int wifi_ps_set_listen_interval(int interval);

/**
 * @brief Disable power save mode.
 *
 * This function disables any active power save mode.
 *
 * @return int Status of the operation.
 */
int wifi_ps_disable(void);






int wifi_ps_set_listen_interval(int interval);

int wifi_ps_enable();

int wifi_ps_wakeup_listen_interval();

int wifi_ps_wakeup_dtim();






#endif /* WIFI_PS_H */