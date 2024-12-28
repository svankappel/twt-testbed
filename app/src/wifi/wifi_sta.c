#include "wifi_sta.h"
#include "wifi_utils.h"
#include "wifi_twt.h"

#include <nrfx_clock.h>
#include <zephyr/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/init.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>

#include <zephyr/net/net_core.h>

#include <net/wifi_mgmt_ext.h>
#include <net/wifi_ready.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(wifi_sta, CONFIG_MY_WIFI_LOG_LEVEL);

#include "net_private.h"

#define WIFI_MGMT_EVENTS (NET_EVENT_WIFI_CONNECT_RESULT | \
								NET_EVENT_WIFI_DISCONNECT_RESULT)

// Function to convert the WiFi arguments to connection parameters


// Semaphore for connection and disconnection events
static K_SEM_DEFINE(connect_sem, 0, 1);
static K_SEM_DEFINE(disconnect_sem, 0, 1);
static K_SEM_DEFINE(dhcp_sem, 0, 1);

//callback structure for WiFi management events
static struct net_mgmt_event_callback wifi_mgmt_cb;
static struct net_mgmt_event_callback net_mgmt_dhcp_cb;

// Declaration of the WiFi disconnected callback function pointer
static void (*wifi_disconnected_cb)(void) = NULL;

// context structure for connection status
static struct {
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


static void handle_wifi_connect_result(struct net_mgmt_event_callback *cb)
{
	// set the connection status
	context.connected = true;
	context.connect_requested = false;

	k_sem_give(&connect_sem);
}


static void handle_wifi_disconnect_result(struct net_mgmt_event_callback *cb)
{
	// Check if the disconnection was requested or not
	if (context.disconnect_requested) {
		context.disconnect_requested = false;
		context.connected = false;
		
		//give semaphore to return from the disconnect function
		k_sem_give(&disconnect_sem);

	} else {
		LOG_WRN("Received Disconnected");
		if (wifi_disconnected_cb) {
			wifi_disconnected_cb();
		}
		context.connected = false;
	}
}


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


static void net_mgmt_dhcp_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface)
{
	switch (mgmt_event) {
	case NET_EVENT_IPV4_DHCP_BOUND:
		k_sem_give(&dhcp_sem);
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

	for(int i = 0; i < 3; i++){		// 3 retries if connect fails

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

		LOG_INF("Connection requested ...");

		// Wait for the connection and DHCP 
		k_sem_take(&connect_sem, K_FOREVER);
		k_sem_take(&dhcp_sem, K_FOREVER);

		// Print the connection status
		if (context.connected) {
			print_wifi_status();
			break;
		}
		else {
			if(i == 2){	
				LOG_ERR("Connection failed\n");
				return -ENOEXEC;
			}

			LOG_WRN("Connection failed. Retrying...\n");
			k_sleep(K_SECONDS(10));
		}
	}

	return 0;
}

/**
 * @brief Initiate a WiFi disconnection
 *
 * @return 0 on success, negative error code on failure
 */
int wifi_disconnect(void)
{
	struct net_if *iface = net_if_get_first_wifi();
	int ret;

	context.disconnect_requested = true;

	ret = net_mgmt(NET_REQUEST_WIFI_DISCONNECT, iface, NULL, 0);

	if (ret) {
		context.disconnect_requested = false;

		if (ret == -EALREADY) {
			LOG_WRN("Already disconnected");
		} else {
			LOG_ERR("Disconnect request failed: %d\n", ret);
			return -ENOEXEC;
		}
	} else {
		LOG_INF("Disconnect requested ...");
	}

	// Wait for the disconnection status
	k_sem_take(&disconnect_sem, K_FOREVER);

	// Print the connection status
	if (context.connected) {
		LOG_ERR("Disonnection failed\n");
		return -ENOEXEC;
	}
	else {
		print_wifi_status();
	}



	return 0;
}

//init function
int wifi_init()
{
	//initialize the context structure
	memset(&context, 0, sizeof(context));

	// Initialize the WiFi management event callbacks
	net_mgmt_init_event_callback(&wifi_mgmt_cb,
								 wifi_mgmt_event_handler,
								 WIFI_MGMT_EVENTS);

	net_mgmt_add_event_callback(&wifi_mgmt_cb);

	net_mgmt_init_event_callback(&net_mgmt_dhcp_cb,
								 net_mgmt_dhcp_event_handler,
								 NET_EVENT_IPV4_DHCP_BOUND);

	net_mgmt_add_event_callback(&net_mgmt_dhcp_cb);

	k_sleep(K_SECONDS(1));
	
	wifi_twt_init();
	
	return 0;
}

void wifi_register_disconnected_cb(void (*cb)(void))
{
	// Register a callback function to be called on disconnection
	if (cb != NULL) {
		// Store the callback function pointer
		wifi_disconnected_cb = cb;
	}
}
