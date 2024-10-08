/**
 * @file wifi_sta.c
 * @brief WiFi Station Management Implementation
 *
 * This file contains the implementation for managing WiFi station
 * connections, including handling connection and disconnection events,
 * and managing DHCP IP assignment.
 *
 * Includes:
 * - Initialization of WiFi management event callbacks
 * - Handling WiFi connection and disconnection results
 * - Handling DHCP IP assignment events
 * - WiFi connection initiation
 * - WiFi initialization
 *
 * Dependencies:
 * - nrfx_clock.h
 * - zephyr/kernel.h
 * - zephyr/shell/shell.h
 * - zephyr/init.h
 * - zephyr/net/net_if.h
 * - zephyr/net/wifi_mgmt.h
 * - zephyr/net/net_event.h
 * - zephyr/net/net_core.h
 * - net/wifi_mgmt_ext.h
 * - net/wifi_ready.h
 * - wifi_utils.h
 * - zephyr/logging/log.h
 *
 * Macros:
 * - WIFI_SHELL_MGMT_EVENTS: Defines the WiFi management events to be handled
 * - STATUS_POLLING_MS: Defines the status polling interval in milliseconds
 *
 * Structures:
 * - context: Holds the shell context and connection status flags
 *
 * Functions:
 * - handle_wifi_connect_result: Handles WiFi connection result events
 * - handle_wifi_disconnect_result: Handles WiFi disconnection result events
 * - wifi_mgmt_event_handler: Handles WiFi management events
 * - net_mgmt_event_handler: Handles network management events
 * - wifi_connect: Initiates a WiFi connection
 * - wifi_init: Initializes WiFi management and starts the connection process
 */
#include "wifi_sta.h"

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

#include "wifi_utils.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(wifi_sta, CONFIG_MY_WIFI_LOG_LEVEL);

#define WIFI_SHELL_MGMT_EVENTS (NET_EVENT_WIFI_CONNECT_RESULT | \
								NET_EVENT_WIFI_DISCONNECT_RESULT)

#define STATUS_POLLING_MS 300

//callback structure for WiFi management events
static struct net_mgmt_event_callback wifi_shell_mgmt_cb;
static struct net_mgmt_event_callback net_shell_mgmt_cb;

// context structure for connection status
static struct {
	const struct shell *sh;
	union {
		struct {
			uint8_t connected : 1;
			uint8_t connect_requested : 1;
			uint8_t disconnect_requested : 1;
			uint8_t _unused : 5;
		};
		uint8_t all;
	};
} context;

/**
 * @brief Handle WiFi connection result events
 *
 * @param cb Pointer to the net_mgmt_event_callback structure
 */
static void handle_wifi_connect_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status =
		(const struct wifi_status *)cb->info;

	// Check if already connected
	if (context.connected) {
		return;
	}

	// Check the connection status
	if (status->status) {
		LOG_ERR("Connection failed (%d)", status->status);
	} else {
		LOG_INF("Connected");
		context.connected = true;
	}

	context.connect_requested = false;
}

/**
 * @brief Handle WiFi disconnection result events
 *
 * @param cb Pointer to the net_mgmt_event_callback structure
 */
static void handle_wifi_disconnect_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status = (const struct wifi_status *)cb->info;

	// Check if already disconnected
	if (!context.connected) {
		return;
	}

	// Check if the disconnection was requested or not
	if (context.disconnect_requested) {
		LOG_INF("Disconnection request %s (%d)",
				status->status ? "failed" : "done",
				status->status);
		context.disconnect_requested = false;
	} else {
		LOG_INF("Received Disconnected");
		context.connected = false;
	}

	cmd_wifi_status();
}

/**
 * @brief Handle WiFi management events
 *
 * @param cb Pointer to the net_mgmt_event_callback structure
 * @param mgmt_event Management event type
 * @param iface Pointer to the network interface
 */
static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface)
{
	// Handle WiFi connection and disconnection events
	switch (mgmt_event) {
	case NET_EVENT_WIFI_CONNECT_RESULT:
		handle_wifi_connect_result(cb);
		break;
	case NET_EVENT_WIFI_DISCONNECT_RESULT:
		handle_wifi_disconnect_result(cb);
		break;
	default:
		break;
	}
}

/**
 * @brief Handle network management events
 *
 * @param cb Pointer to the net_mgmt_event_callback structure
 * @param mgmt_event Management event type
 * @param iface Pointer to the network interface
 */
static void net_mgmt_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface)
{
	//print the DHCP IP address when the device is bound
	switch (mgmt_event) {
	case NET_EVENT_IPV4_DHCP_BOUND:
		print_dhcp_ip(cb);
		break;
	default:
		break;
	}
}


/**
 * @brief Initiate a WiFi connection
 *
 * @return 0 on success, negative error code on failure
 */
int wifi_connect(void)
{
	// Get the WiFi network interface
	struct net_if *iface = net_if_get_first_wifi();
	struct wifi_connect_req_params cnx_params = { 0 };
	int ret;

	wifi_args_to_params(&cnx_params);

	context.connected = false;
	context.connect_requested = true;

	ret = net_mgmt(NET_REQUEST_WIFI_CONNECT, iface,
		       &cnx_params, sizeof(struct wifi_connect_req_params));
	if (ret) {
		printk("Connection request failed with error: %d\n", ret);
		context.connect_requested = false;
		return -ENOEXEC;
	}

	LOG_INF("Connection requested\n");

	// Poll the connection status until connected
	while (context.connect_requested) {
		cmd_wifi_status();
		k_sleep(K_MSEC(STATUS_POLLING_MS));
	}

	// Print the connection status
	if (context.connected) {
		cmd_wifi_status();
	}

	return 0;
}


//init function
int wifi_init()
{
	//initialize the context structure
	memset(&context, 0, sizeof(context));

	// Initialize the WiFi management event callbacks
	net_mgmt_init_event_callback(&wifi_shell_mgmt_cb,
								 wifi_mgmt_event_handler,
								 WIFI_SHELL_MGMT_EVENTS);

	net_mgmt_add_event_callback(&wifi_shell_mgmt_cb);

	net_mgmt_init_event_callback(&net_shell_mgmt_cb,
								 net_mgmt_event_handler,
								 NET_EVENT_IPV4_DHCP_BOUND);

	net_mgmt_add_event_callback(&net_shell_mgmt_cb);

	k_sleep(K_SECONDS(1));
	
	return 0;
}
