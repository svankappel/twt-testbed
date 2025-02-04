/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file wifi_ps.h
 * @brief Wi-Fi Power Save Mode 
 *
 * This header file provides functions for configuring Wi-Fi power save modes.
 */
#ifndef WIFI_PS_H
#define WIFI_PS_H

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

#endif // WIFI_PS_H