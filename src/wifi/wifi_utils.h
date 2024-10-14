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
#include <zephyr/net/wifi_mgmt.h>

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

/**
 * @brief Convert WiFi arguments to connection parameters.
 *
 * This function converts the provided WiFi connection arguments into a 
 * `wifi_connect_req_params` structure, which can then be used to initiate 
 * a WiFi connection.
 *
 * @param params Pointer to the WiFi connection request parameters structure.
 */
void wifi_args_to_params(struct wifi_connect_req_params *params);

/**
 * @brief Print the negotiated TWT parameters.
 *
 * This function prints the negotiated TWT parameters received in the TWT response.
 *
 * @param resp Pointer to the WiFi TWT parameters structure.
 */
void print_twt_negotiated_params(const struct wifi_twt_params *resp);

#endif // WIFI_UTILS_H