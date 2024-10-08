/**
 * @file wifi_sta.h
 * @brief Header file for WiFi station initialization.
 *
 * This file contains the declaration for the WiFi station initialization function.
 */


#ifndef WIFI_STA_H
#define WIFI_STA_H

 /**
 * @brief Initializes the WiFi station.
 *
 * This function sets up the WiFi station mode and connects to the network specified
 * in the configuration.
 *
 * @return int Returns 0 on success, or a negative error code on failure.
 */
int wifi_init(void);


#endif // WIFI_STA_H