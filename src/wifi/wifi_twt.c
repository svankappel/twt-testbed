#include "wifi_twt.h"


#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <zephyr/logging/log.h>

#include <zephyr/net/wifi.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_mgmt.h>
#include <net/wifi_mgmt_ext.h>
#include <net/wifi_credentials.h>
#include <zephyr/net/socket.h>

#include "wifi_utils.h"

#define TWT_POLLING_MS 300

LOG_MODULE_REGISTER(wifi_twt, CONFIG_MY_WIFI_LOG_LEVEL); // Register the logging module


#define TWT_MGMT_EVENTS (NET_EVENT_WIFI_TWT | NET_EVENT_WIFI_TWT_SLEEP_STATE)

bool twt_enabled = false;

bool twt_request_pending = false;

static uint32_t twt_flow_id = 0;

void (*twt_event_callback)(const int awake) = NULL;

void twt_register_event_callback(void (*callback)(const int awake)) {
	twt_event_callback = callback;
}


static struct net_mgmt_event_callback twt_mgmt_cb;


static void handle_wifi_twt_event(struct net_mgmt_event_callback *cb)
{
	// Create a wifi_twt_params struct for the received TWT event and fill it with the event information.
	const struct wifi_twt_params *resp = (const struct wifi_twt_params *)cb->info;

	// If the event was a TWT teardown initiated by the AP, set change the value of twt_enabled and exit the function.
	if (resp->operation == WIFI_TWT_TEARDOWN) {
		LOG_INF("TWT teardown received for flow ID %d\n", resp->flow_id);
		twt_enabled = false;
		twt_request_pending = false;
		return;
	}

	// Update twt_flow_id to reflect the flow ID received in the TWT response.
	twt_flow_id = resp->flow_id;

	// Check if a TWT response was received. If not, the TWT request timed out.
	if (resp->resp_status == WIFI_TWT_RESP_RECEIVED) {
		LOG_INF("TWT response: %s", wifi_twt_setup_cmd_txt(resp->setup_cmd));
	} else {
		LOG_INF("TWT response timed out\n");
		return;
	}
	// If the TWT setup was accepted, change the value of twt_enabled and print the negotiated parameters.
	if (resp->setup_cmd == WIFI_TWT_SETUP_CMD_ACCEPT) {
		twt_enabled = true;
		twt_request_pending = false;
		print_twt_negotiated_params(resp);
	}
}

static void twt_mgmt_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event,
				   struct net_if *iface)
{
	switch (mgmt_event) {
	case NET_EVENT_WIFI_TWT:
		handle_wifi_twt_event(cb);
		break;
	case NET_EVENT_WIFI_TWT_SLEEP_STATE:
		int *twt_state;
		twt_state = (int *)(cb->info);
		if (twt_event_callback) {
			twt_event_callback(*twt_state);
		}
		break;
	
	}
}

int twt_setup(uint32_t twt_wake_interval_ms, uint32_t twt_interval_ms)
{
	//get interface
	struct net_if *iface = net_if_get_first_wifi();

	// Define the TWT parameters struct wifi_twt_params and fill the parameters for TWT setup.
	struct wifi_twt_params params = {0};

	params.negotiation_type = WIFI_TWT_INDIVIDUAL;
	params.flow_id = twt_flow_id;
	params.dialog_token = 1;
	params.operation = WIFI_TWT_SETUP;
	params.setup_cmd = WIFI_TWT_SETUP_CMD_REQUEST;
	params.setup.responder = 0;
	params.setup.trigger = 0;
	params.setup.implicit = 1;
	params.setup.announce = 0;
	params.setup.twt_wake_interval = twt_wake_interval_ms * USEC_PER_MSEC;
	params.setup.twt_interval = twt_interval_ms * USEC_PER_MSEC;

	// Send the TWT setup request with net_mgmt.
	if (net_mgmt(NET_REQUEST_WIFI_TWT, iface, &params, sizeof(params))) {
		LOG_ERR("TWT setup with %s failed, reason : %s",
			wifi_twt_negotiation_type_txt(params.negotiation_type),
			wifi_twt_get_err_code_str(params.fail_reason));
		return -1;
	}
	twt_request_pending = true;
	LOG_INF("-------------------------------");
	LOG_INF("TWT setup requested");
	LOG_INF("-------------------------------");

	// Poll the twt status until connected
	while(twt_request_pending) {
		k_sleep(K_MSEC(TWT_POLLING_MS));
	}

	if(!twt_enabled) {
		LOG_ERR("TWT setup failed");
		return -1;
	}
	
	return 0;
}

int twt_teardown()
{
	//get interface
	struct net_if *iface = net_if_get_first_wifi();

	// Define the TWT parameters struct wifi_twt_params and fill the parameters for TWT teardown.
	struct wifi_twt_params params = {0};

	params.negotiation_type = WIFI_TWT_INDIVIDUAL;
	params.flow_id = twt_flow_id;
	params.dialog_token = 1;
	params.operation = WIFI_TWT_TEARDOWN;
	params.setup_cmd = WIFI_TWT_TEARDOWN;

	// Send the TWT teardown request with net_mgmt.
	if (net_mgmt(NET_REQUEST_WIFI_TWT, iface, &params, sizeof(params))) {
		LOG_ERR("TWT teardown with %s failed, reason : %s",
			wifi_twt_negotiation_type_txt(params.negotiation_type),
			wifi_twt_get_err_code_str(params.fail_reason));
		return -1;
	}

	twt_request_pending = true;

	LOG_INF("-------------------------------");
	LOG_INF("TWT teardown requested");
	LOG_INF("-------------------------------");

	// Poll the twt status until connected
	while(twt_request_pending) {
		k_sleep(K_MSEC(TWT_POLLING_MS));
	}

	if(twt_enabled) {
		LOG_ERR("TWT setup failed");
		return -1;
	}
	
	// Update flow ID and disable TWT.
	twt_flow_id = twt_flow_id < WIFI_MAX_TWT_FLOWS-1 ? twt_flow_id + 1 : 0;

	return 0;
}

int twt_init()
{
	net_mgmt_init_event_callback(&twt_mgmt_cb, twt_mgmt_event_handler, TWT_MGMT_EVENTS);
	net_mgmt_add_event_callback(&twt_mgmt_cb);
}