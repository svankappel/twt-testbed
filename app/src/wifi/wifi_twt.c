/*
 * Copyright (c) 2025 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/**
 * @file wifi_twt.c
 * @brief Source file for TWT (Target Wake Time) Wi-Fi operations.
 *
 * This file contains the functions used to initialize,
 * set up, and tear down TWT operations in a Wi-Fi application.
 */

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

LOG_MODULE_REGISTER(wifi_twt, CONFIG_MY_WIFI_LOG_LEVEL); // Register the logging module

#define TWT_MGMT_EVENTS (NET_EVENT_WIFI_TWT | NET_EVENT_WIFI_TWT_SLEEP_STATE)

static bool twt_enabled = false;

static K_SEM_DEFINE(twt_teardown_sem, 0, 1);
static bool twt_teardown_requested = false;
static K_SEM_DEFINE(twt_setup_sem, 0, 1);

static uint32_t twt_flow_id = 0;
static uint16_t twt_dialog_token = 1;

static uint32_t wake_ahead_ms = 0;
static uint32_t twt_interval_ms = 0;
static uint32_t twt_wake_interval_ms = 0;

static struct wifi_twt_params params = {0};


static struct net_mgmt_event_callback twt_mgmt_cb;

static void wifi_twt_ahead_callback();

static K_TIMER_DEFINE(wake_ahead_timer, wifi_twt_ahead_callback, NULL);

static void (*twt_event_callback)() = NULL;

void wifi_twt_register_event_callback(void (*callback)(), uint32_t wake_ahead) {
	twt_event_callback = callback;
	wake_ahead_ms = wake_ahead;
}

static void wifi_twt_ahead_callback()
{
	(*twt_event_callback)();
}

static void wifi_handle_wifi_twt_event(struct net_mgmt_event_callback *cb)
{
	// Create a wifi_twt_params struct for the received TWT event and fill it with the event information.
	const struct wifi_twt_params *resp = (const struct wifi_twt_params *)cb->info;

	// If the event was a TWT teardown initiated by the AP, set change the value of twt_enabled and exit the function.
	if (resp->operation == WIFI_TWT_TEARDOWN) {
		LOG_INF("TWT teardown received for flow ID %d", resp->flow_id);
		LOG_INF("-------------------------------");
	
		twt_enabled = false;
		
		if(twt_teardown_requested){
			twt_teardown_requested = false;
			k_sem_give(&twt_teardown_sem);
		}
		return;
	}


	// Check if a TWT response was received. If not, the TWT request timed out.
	if (resp->resp_status != WIFI_TWT_RESP_RECEIVED) {
		LOG_ERR("TWT response timed out\n");
		LOG_INF("-------------------------------");
		return;
	}
	// If the TWT setup was accepted, change the value of twt_enabled and print the negotiated parameters.
	if (resp->setup_cmd == WIFI_TWT_SETUP_CMD_ACCEPT) {
		twt_enabled = true;
		LOG_INF("TWT response: %s", wifi_twt_setup_cmd_txt(resp->setup_cmd));
		print_twt_negotiated_params(resp);

		//print warning if negotiated values are different from requested values
		if(params.setup.implicit != resp->setup.implicit)
		{
			LOG_WRN("Implicit value negotiated is different from requested");
		}
		if(params.setup.announce != resp->setup.announce)
		{
			LOG_WRN("Announce value negotiated is different from requested");
		}
		if(params.setup.trigger != resp->setup.trigger)
		{
			LOG_WRN("Trigger value negotiated is different from requested");
		}
		LOG_INF("-------------------------------");

		twt_interval_ms = resp->setup.twt_interval / USEC_PER_MSEC;
		twt_wake_interval_ms = resp->setup.twt_wake_interval / USEC_PER_MSEC;
		k_sem_give(&twt_setup_sem);
		return;
	}
}

static void wifi_twt_mgmt_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event,
				   struct net_if *iface)
{
	switch (mgmt_event) {
	case NET_EVENT_WIFI_TWT:
		wifi_handle_wifi_twt_event(cb);
		break;
	case NET_EVENT_WIFI_TWT_SLEEP_STATE:
		int *twt_state;
		twt_state = (int *)(cb->info);
		if (twt_event_callback) {
			if(*twt_state == WIFI_TWT_STATE_SLEEP)
			{
				k_timer_start(&wake_ahead_timer, K_MSEC(twt_interval_ms - twt_wake_interval_ms - wake_ahead_ms), K_NO_WAIT);
			}
		}
		break;
	
	}
}

int wifi_twt_setup(uint32_t twt_wake_interval_ms, uint32_t twt_interval_ms)
{
	//get interface
	struct net_if *iface = net_if_get_first_wifi();

	// Define the TWT parameters struct wifi_twt_params and fill the parameters for TWT setup.

	params.negotiation_type = WIFI_TWT_INDIVIDUAL;
	params.flow_id = twt_flow_id;
	params.dialog_token = twt_dialog_token > 255 ? 1 : twt_dialog_token++;
	params.operation = WIFI_TWT_SETUP;
	params.setup_cmd = WIFI_TWT_SETUP_CMD_REQUEST;
	params.setup.responder = 0;
	
	#ifdef CONFIG_TWT_TRIGGER
	params.setup.trigger = 1;
	#else
	params.setup.trigger = 0;
	#endif

	#ifdef CONFIG_TWT_IMPLICIT
	params.setup.implicit = 1;
	#else
	params.setup.implicit = 0;
	#endif

	#ifdef CONFIG_TWT_ANNOUNCED_MODE
	params.setup.announce = 1;
	#else
	params.setup.announce = 0;
	#endif

	params.setup.twt_wake_interval = twt_wake_interval_ms * USEC_PER_MSEC;
	params.setup.twt_interval = twt_interval_ms * USEC_PER_MSEC;

	for(int i = 0; i < 3; i++){		// 3 retries if setup fails
		// Send the TWT setup request with net_mgmt.
		if (net_mgmt(NET_REQUEST_WIFI_TWT, iface, &params, sizeof(params))) {
			LOG_ERR("TWT setup with %s failed, reason : %s",
				wifi_twt_negotiation_type_txt(params.negotiation_type),
				wifi_twt_get_err_code_str(params.fail_reason));
			return -1;
		}
		
		LOG_INF("-------------------------------");
		LOG_INF("TWT setup requested");
		LOG_INF("-------------------------------");

		// wait until setup is complete
		if (k_sem_take(&twt_setup_sem, K_SECONDS(5)) != 0) {
			LOG_ERR("TWT setup timed out");
			continue;
		}
		break;
	}

	if(!twt_enabled) {
		LOG_ERR("TWT setup failed");
		return -1;
	}
	
	return 0;
}

int wifi_twt_teardown()
{
	//get interface
	struct net_if *iface = net_if_get_first_wifi();

	// Define the TWT parameters struct wifi_twt_params and fill the parameters for TWT teardown.

	params.negotiation_type = WIFI_TWT_INDIVIDUAL;
	params.flow_id = twt_flow_id;
	params.dialog_token = twt_dialog_token > 255 ? 1 : twt_dialog_token++;
	params.operation = WIFI_TWT_TEARDOWN;
	params.setup_cmd = WIFI_TWT_TEARDOWN;

	LOG_INF("-------------------------------");
	LOG_INF("TWT teardown requested");
	LOG_INF("-------------------------------");
	
	for(int i = 0; i < 3; i++){		// 3 retries if teardown fails
		// Send the TWT teardown request with net_mgmt.
		if (net_mgmt(NET_REQUEST_WIFI_TWT, iface, &params, sizeof(params))) {
			LOG_ERR("TWT teardown with %s failed, reason : %s",
				wifi_twt_negotiation_type_txt(params.negotiation_type),
				wifi_twt_get_err_code_str(params.fail_reason));
			return -1;
		}

		// wait until teardown is complete
		twt_teardown_requested = true;
		if(k_sem_take(&twt_teardown_sem, K_FOREVER)){
			LOG_ERR("TWT setup timed out");
			continue;
		}
		break;
	}
	

	if(twt_enabled) {
		LOG_ERR("TWT teardown failed");
		return -1;
	}
	
	// Update flow ID
	twt_flow_id = twt_flow_id < WIFI_MAX_TWT_FLOWS-1 ? twt_flow_id + 1 : 0;

	return 0;
}

void wifi_twt_init()
{
	net_mgmt_init_event_callback(&twt_mgmt_cb, wifi_twt_mgmt_event_handler, TWT_MGMT_EVENTS);
	net_mgmt_add_event_callback(&twt_mgmt_cb);
}

bool wifi_twt_is_enabled()
{
	return twt_enabled;
}

uint32_t wifi_twt_get_interval_ms(void) {
	return twt_interval_ms;
}

uint32_t wifi_twt_get_wake_interval_ms(void) {
	return twt_wake_interval_ms;
}