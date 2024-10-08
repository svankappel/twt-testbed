/**
 * @file wifi_utils.h
 * @brief WiFi Utility Functions Header
 *
 * This header file contains declarations for WiFi utility functions used in the application.
 * It includes functions to get the WiFi status and print the DHCP IP address.
 *
 */

#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include <zephyr/net/net_if.h> // Include Zephyr network interface API

/**
 * @brief Command to get the WiFi status.
 *
 * This function retrieves the current status of the WiFi interface and logs
 * various details such as state, mode, SSID, and BSSID.
 *
 * @return 0 on success, -ENOEXEC on failure.
 */
int cmd_wifi_status(void); 

/**
 * @brief Print DHCP IP address.
 *
 * This function prints the IP address obtained via DHCP.
 *
 * @param cb Pointer to the network management event callback structure.
 */
void print_dhcp_ip(struct net_mgmt_event_callback *cb);

#endif // WIFI_UTILS_H