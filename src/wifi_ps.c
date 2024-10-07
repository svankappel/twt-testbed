#include "wifi_ps.h"

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

LOG_MODULE_REGISTER(wifi_ps, CONFIG_MY_WIFI_LOG_LEVEL);


int wifi_ps_legacy_dtim()
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.enabled = WIFI_PS_ENABLED;
	params.mode = WIFI_PS_MODE_LEGACY;
	params.wakeup_mode = WIFI_PS_WAKEUP_MODE_DTIM;
	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save %s failed. Reason %s", params.enabled ? "enable" : "disable",
			wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save: %s", params.enabled ? "enable" : "disable");

	return 0;
}

int wifi_ps_wmm_dtim()
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.enabled = WIFI_PS_ENABLED;
	params.mode = WIFI_PS_MODE_WMM;
	params.wakeup_mode = WIFI_PS_WAKEUP_MODE_DTIM;
	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save %s failed. Reason %s", params.enabled ? "enable" : "disable",
			wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save: %s", params.enabled ? "enable" : "disable");

	return 0;
}

int wifi_ps_legacy_listen_interval(int interval)
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.enabled = WIFI_PS_ENABLED;
	params.mode = WIFI_PS_MODE_LEGACY;
	params.wakeup_mode = WIFI_PS_WAKEUP_MODE_LISTEN_INTERVAL;
	params.listen_interval = interval;
	
	
	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save %s failed. Reason %s", params.enabled ? "enable" : "disable",
			wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save: %s", params.enabled ? "enable" : "disable");

	return 0;
}

int wifi_ps_wmm_listen_interval(int interval)
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.enabled = WIFI_PS_ENABLED;
	params.mode = WIFI_PS_MODE_WMM;
	params.wakeup_mode = WIFI_PS_WAKEUP_MODE_LISTEN_INTERVAL;
	params.listen_interval = interval;

	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save %s failed. Reason %s", params.enabled ? "enable" : "disable",
			wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save: %s", params.enabled ? "enable" : "disable");

	return 0;
}

int wifi_ps_disable()
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.enabled = WIFI_PS_DISABLED;

	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save %s failed. Reason %s", params.enabled ? "enable" : "disable",
			wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save: %s", params.enabled ? "enable" : "disable");

	return 0;
}