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

int wifi_ps_mode_legacy()
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.mode = WIFI_PS_MODE_LEGACY;
	params.type = WIFI_PS_PARAM_MODE;

	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save legacy mode failed. Reason %s", wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save mode : legacy");

	return 0;
}

int wifi_ps_mode_wmm()
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.mode = WIFI_PS_MODE_WMM;
	params.type = WIFI_PS_PARAM_MODE;

	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save WMM mode failed. Reason %s", wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save mode : WMM");

	return 0;
}

int wifi_ps_wakeup_dtim()
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.wakeup_mode = WIFI_PS_WAKEUP_MODE_DTIM;
	params.type = WIFI_PS_PARAM_WAKEUP_MODE;
	
	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save wakeup mode DTIM failed. Reason %s", wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save wakeup mode : DTIM");

	return 0;
}

int wifi_ps_wakeup_listen_interval()
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.wakeup_mode = WIFI_PS_WAKEUP_MODE_LISTEN_INTERVAL;
	params.type = WIFI_PS_PARAM_WAKEUP_MODE;

	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save wakeup mode listen interval failed. Reason %s", wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save wakeup mode : listen interval");

	return 0;
}

int wifi_ps_enable()
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.enabled = WIFI_PS_ENABLED;
	params.type = WIFI_PS_PARAM_STATE;

	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save disable failed. Reason %s", wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save: enable");

	return 0;
}

int wifi_ps_disable()
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.enabled = WIFI_PS_DISABLED;
	params.type = WIFI_PS_PARAM_STATE;

	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save disable failed. Reason %s", wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save: disable");

	return 0;
}

int wifi_ps_set_listen_interval(int interval)
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.listen_interval = interval;
	params.type = WIFI_PS_PARAM_LISTEN_INTERVAL;
	
	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save set listen interval failed. Reason %s", wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set listen interval: %d", interval);

	return 0;
}
