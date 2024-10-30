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
LOG_MODULE_REGISTER(wifi_uti, CONFIG_MY_WIFI_LOG_LEVEL); // Register the logging module


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
	
	struct in_addr *ipv4 = &iface->config.ip.ipv4->unicast[0].ipv4.address.in_addr; // Get the IPv4 address
	struct in_addr6 *ipv6 = &iface->config.ip.ipv6->unicast[0].address.in6_addr; // Get the IPv6 address

	char ipv4_addr[NET_IPV4_ADDR_LEN]; // Buffer to store the IP address
	char ipv6_addr[NET_IPV6_ADDR_LEN]; // Buffer to store the IP address

	net_addr_ntop(AF_INET, ipv4, ipv4_addr, sizeof(ipv4_addr)); // Convert IP address to string
	net_addr_ntop(AF_INET6, ipv6, ipv6_addr, sizeof(ipv6_addr)); // Convert IP address to string

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
		LOG_INF("IPv4 address: %s", ipv4_addr); 
		LOG_INF("IPv6 address: %s", ipv6_addr); 

		if(status.link_mode < WIFI_6){
			LOG_WRN("AP version lower than 802.11ax - Link mode: %s", wifi_link_mode_txt(status.link_mode));
		}
		if(!status.twt_capable){
			LOG_WRN("AP does not support TWT");
		}
	}
	LOG_INF("==================");
	return 0;
}


//wifi args to params
void wifi_args_to_params(struct wifi_connect_req_params *params)
{
	// Populate the SSID and password
	params->ssid = CONFIG_WIFI_CREDENTIALS_STATIC_SSID;
	params->ssid_length = strlen(params->ssid);

	params->psk = CONFIG_WIFI_CREDENTIALS_STATIC_PASSWORD;
	params->psk_length = strlen(params->psk);

	// Populate the rest of the relevant members
	params->channel = WIFI_CHANNEL_ANY;
	params->security = WIFI_SECURITY_TYPE_PSK;
	params->mfp = WIFI_MFP_OPTIONAL;
	params->timeout = SYS_FOREVER_MS;
	params->band = WIFI_FREQ_BAND_UNKNOWN;
}

//wifi twt print negotiated params
void print_twt_negotiated_params(const struct wifi_twt_params *resp)
{
	LOG_INF("== TWT negotiated parameters ==");
	LOG_INF("TWT Dialog token: %d", resp->dialog_token);
	LOG_INF("TWT flow ID: %d", resp->flow_id);
	LOG_INF("TWT negotiation type: %s", wifi_twt_negotiation_type_txt(resp->negotiation_type));
	LOG_INF("TWT responder: %s", resp->setup.responder ? "true" : "false");
	LOG_INF("TWT implicit: %s", resp->setup.implicit ? "true" : "false");
	LOG_INF("TWT announce: %s", resp->setup.announce ? "true" : "false");
	LOG_INF("TWT trigger: %s", resp->setup.trigger ? "true" : "false");
	LOG_INF("TWT wake interval: %d ms (%d us)", resp->setup.twt_wake_interval / USEC_PER_MSEC, resp->setup.twt_wake_interval);
	LOG_INF("TWT interval: %lld s (%lld us)", resp->setup.twt_interval / USEC_PER_SEC, resp->setup.twt_interval);
}