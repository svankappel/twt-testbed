/**
 * @file wifi_utils.c
 * @brief WiFi utils for wifi_sta
 *
 * This file contains utility functions used by wifi_sta
 * 
 */

#include "wifi_utils.h"

#include <nrfx_clock.h>
#include <zephyr/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/shell/shell.h>
#include <zephyr/init.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>

#include <zephyr/net/net_core.h>

#include <net/wifi_mgmt_ext.h>
#include <net/wifi_ready.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(wifi_utils, CONFIG_MY_WIFI_LOG_LEVEL); // Register the logging module



// retrieves and logs the current status of the WiFi interface.
 
int cmd_wifi_status(void)
{
	struct net_if *iface = net_if_get_default(); // Get the default network interface
	struct wifi_iface_status status = { 0 }; // Initialize the WiFi interface status structure

	// Request the WiFi interface status
	if (net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS, iface, &status, sizeof(struct wifi_iface_status))) {
		LOG_INF("Status request failed");
		return -ENOEXEC; // Return error if status request fails
	}

	// Log the WiFi status information
	LOG_INF("==================");
	LOG_INF("State: %s", wifi_state_txt(status.state));

	if (status.state >= WIFI_STATE_ASSOCIATED) {
		uint8_t mac_string_buf[sizeof("xx:xx:xx:xx:xx:xx")];

		LOG_INF("Interface Mode: %s", wifi_mode_txt(status.iface_mode));
		LOG_INF("Link Mode: %s", wifi_link_mode_txt(status.link_mode));
		LOG_INF("SSID: %.32s", status.ssid);
		LOG_INF("BSSID: %s", net_sprint_ll_addr_buf(status.bssid, WIFI_MAC_ADDR_LEN, mac_string_buf, sizeof(mac_string_buf)));
		LOG_INF("Band: %s", wifi_band_txt(status.band));
		LOG_INF("Channel: %d", status.channel);
		LOG_INF("Security: %s", wifi_security_txt(status.security));
		LOG_INF("MFP: %s", wifi_mfp_txt(status.mfp));
		LOG_INF("RSSI: %d", status.rssi);
	}
	return 0;
}


//print IP
void print_dhcp_ip(struct net_mgmt_event_callback *cb)
{
	// Get DHCP info from struct net_if_dhcpv4 and print
	const struct net_if_dhcpv4 *dhcpv4 = cb->info;
	const struct in_addr *addr = &dhcpv4->requested_ip;
	char dhcp_info[128];

	net_addr_ntop(AF_INET, addr, dhcp_info, sizeof(dhcp_info)); // Convert IP address to string

	LOG_INF("DHCP IP address: %s", dhcp_info); // Log the DHCP IP address
}