#include "wifi.h"

LOG_MODULE_REGISTER(wifi,CONFIG_MY_WIFI_LOG_LEVEL);

// Macros used to subscribe to specific Zephyr NET management events. 
#define L4_EVENT_MASK (NET_EVENT_L4_CONNECTED | NET_EVENT_L4_DISCONNECTED)

// Zephyr NET management event callback structures. 
static struct net_mgmt_event_callback l4_cb;

// Variable used to indicate if network is connected. 
static bool is_connected;

// Mutex and conditional variable used to signal network connectivity.
K_MUTEX_DEFINE(network_connected_lock);
K_CONDVAR_DEFINE(network_connected);

void wait_for_network(void)
{
	k_mutex_lock(&network_connected_lock, K_FOREVER);

	if (!is_connected) {
		LOG_INF("Waiting for network connectivity");
		k_condvar_wait(&network_connected, &network_connected_lock, K_FOREVER);
	}

	k_mutex_unlock(&network_connected_lock);
}

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

int wifi_ps_legacy_listen_interval()
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.enabled = WIFI_PS_ENABLED;
	params.mode = WIFI_PS_MODE_LEGACY;
	params.wakeup_mode = WIFI_PS_WAKEUP_MODE_LISTEN_INTERVAL;
	params.listen_interval = 10;
	// Send the power save request with net_mgmt. 
	if (net_mgmt(NET_REQUEST_WIFI_PS, iface, &params, sizeof(params))) {
		LOG_ERR("Power save %s failed. Reason %s", params.enabled ? "enable" : "disable",
			wifi_ps_get_config_err_code_str(params.fail_reason));
		return -1;
	}
	LOG_INF("Set power save: %s", params.enabled ? "enable" : "disable");

	return 0;
}

int wifi_ps_wmm_listen_interval()
{
	struct net_if *iface = net_if_get_first_wifi();

	// Define the Wi-Fi power save parameters struct wifi_ps_params.
	struct wifi_ps_params params = {0};

	params.enabled = WIFI_PS_ENABLED;
	params.mode = WIFI_PS_MODE_WMM;
	params.wakeup_mode = WIFI_PS_WAKEUP_MODE_LISTEN_INTERVAL;
	params.listen_interval = 10;
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

static int wifi_args_to_params(struct wifi_connect_req_params *params)
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
	#if NCS_VERSION_NUMBER > 0x20600
	memset(params->bssid, 0, sizeof(params->bssid));
	#endif

	return 0;
}

static void l4_event_handler(struct net_mgmt_event_callback *cb,
				 uint32_t event,
				 struct net_if *iface)
{
	switch (event) {
	case NET_EVENT_L4_CONNECTED:
		LOG_INF("Network connectivity established");
		k_mutex_lock(&network_connected_lock, K_FOREVER);
		is_connected = true;
		k_condvar_signal(&network_connected);
		k_mutex_unlock(&network_connected_lock);
		break;
	case NET_EVENT_L4_DISCONNECTED:
		LOG_INF("Network connectivity lost");
		k_mutex_lock(&network_connected_lock, K_FOREVER);
		is_connected = false;
		k_mutex_unlock(&network_connected_lock);
		break;
	default:
		/* Don't care */
		return;
	}
}

int wifi_init(){
	int err;
	// Declare the variable for the network configuration parameters
	struct wifi_connect_req_params cnx_params;

	// Get the network interface
	struct net_if *iface = net_if_get_first_wifi();
	if (iface == NULL) {
		LOG_ERR("Returned network interface is NULL");
		return -1;
	}

	// Sleep to allow initialization of Wi-Fi driver
	k_sleep(K_SECONDS(1));

	// Setup handler for Zephyr NET Connection Manager events and Connectivity layer.
	net_mgmt_init_event_callback(&l4_cb, l4_event_handler, L4_EVENT_MASK);
	net_mgmt_add_event_callback(&l4_cb);

	// Populate cnx_params with the network configuration
	wifi_args_to_params(&cnx_params);

	// Call net_mgmt() to request the Wi-Fi connection
	err = net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &cnx_params, sizeof(struct wifi_connect_req_params));
	if (err) {
		LOG_ERR("Connecting to Wi-Fi failed, err: %d", err);
		return ENOEXEC;
	}

	return 0;
}
