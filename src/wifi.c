#include "wifi.h"

LOG_MODULE_REGISTER(wifi_sta,CONFIG_MY_WIFI_LOG_LEVEL);

#define WIFI_SHELL_MODULE "wifi"

#define WIFI_SHELL_MGMT_EVENTS (NET_EVENT_WIFI_CONNECT_RESULT |		\
				NET_EVENT_WIFI_DISCONNECT_RESULT)

#define MAX_SSID_LEN        32
#define STATUS_POLLING_MS   300

static struct net_mgmt_event_callback wifi_shell_mgmt_cb;
static struct net_mgmt_event_callback net_shell_mgmt_cb;

static K_SEM_DEFINE(wifi_ready_state_changed_sem, 0, 1);
static bool wifi_ready_status;


static struct {
	const struct shell *sh;
	union {
		struct {
			uint8_t connected	: 1;
			uint8_t connect_result	: 1;
			uint8_t disconnect_requested	: 1;
			uint8_t _unused		: 5;
		};
		uint8_t all;
	};
} context;

static int cmd_wifi_status(void)
{
	struct net_if *iface = net_if_get_default();
	struct wifi_iface_status status = { 0 };

	if (net_mgmt(NET_REQUEST_WIFI_IFACE_STATUS, iface, &status,
				sizeof(struct wifi_iface_status))) {
		LOG_INF("Status request failed");

		return -ENOEXEC;
	}

	LOG_INF("==================");
	LOG_INF("State: %s", wifi_state_txt(status.state));

	if (status.state >= WIFI_STATE_ASSOCIATED) {
		uint8_t mac_string_buf[sizeof("xx:xx:xx:xx:xx:xx")];

		LOG_INF("Interface Mode: %s",
		       wifi_mode_txt(status.iface_mode));
		LOG_INF("Link Mode: %s",
		       wifi_link_mode_txt(status.link_mode));
		LOG_INF("SSID: %.32s", status.ssid);
		//LOG_INF("BSSID: %s", net_sprint_ll_addr_buf(status.bssid, WIFI_MAC_ADDR_LEN, mac_string_buf, sizeof(mac_string_buf)));
		LOG_INF("Band: %s", wifi_band_txt(status.band));
		LOG_INF("Channel: %d", status.channel);
		LOG_INF("Security: %s", wifi_security_txt(status.security));
		LOG_INF("MFP: %s", wifi_mfp_txt(status.mfp));
		LOG_INF("RSSI: %d", status.rssi);
	}
	return 0;
}


static void handle_wifi_connect_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status =
		(const struct wifi_status *) cb->info;

	if (context.connected) {
		return;
	}

	if (status->status) {
		LOG_ERR("Connection failed (%d)", status->status);
	} else {
		LOG_INF("Connected");
		context.connected = true;
	}

	context.connect_result = true;
}

static void handle_wifi_disconnect_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status =
		(const struct wifi_status *) cb->info;

	if (!context.connected) {
		return;
	}

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


static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface)
{
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

static void print_dhcp_ip(struct net_mgmt_event_callback *cb)
{
	/* Get DHCP info from struct net_if_dhcpv4 and print */
	const struct net_if_dhcpv4 *dhcpv4 = cb->info;
	const struct in_addr *addr = &dhcpv4->requested_ip;
	char dhcp_info[128];

	net_addr_ntop(AF_INET, addr, dhcp_info, sizeof(dhcp_info));

	LOG_INF("DHCP IP address: %s", dhcp_info);
}

static int wifi_connect(void)
{
	struct net_if *iface = net_if_get_first_wifi();

	context.connected = false;
	context.connect_result = false;

	if (net_mgmt(NET_REQUEST_WIFI_CONNECT_STORED, iface, NULL, 0)) {
		LOG_ERR("Connection request failed");

		return -ENOEXEC;
	}

	LOG_INF("Connection requested");

	return 0;
}


static void net_mgmt_event_handler(struct net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface)
{
	switch (mgmt_event) {
	case NET_EVENT_IPV4_DHCP_BOUND:
		print_dhcp_ip(cb);
		break;
	default:
		break;
	}
}



void net_mgmt_callback_init(void)
{
	memset(&context, 0, sizeof(context));

	net_mgmt_init_event_callback(&wifi_shell_mgmt_cb,
				     wifi_mgmt_event_handler,
				     WIFI_SHELL_MGMT_EVENTS);

	net_mgmt_add_event_callback(&wifi_shell_mgmt_cb);

	net_mgmt_init_event_callback(&net_shell_mgmt_cb,
				     net_mgmt_event_handler,
				     NET_EVENT_IPV4_DHCP_BOUND);

	net_mgmt_add_event_callback(&net_shell_mgmt_cb);

	LOG_INF("Starting %s with CPU frequency: %d MHz", CONFIG_BOARD, SystemCoreClock/MHZ(1));
	k_sleep(K_SECONDS(1));
}



void start_wifi_thread(void);

K_THREAD_DEFINE(start_wifi_thread_id, 4096,
		start_wifi_thread, NULL, NULL, NULL,
		K_PRIO_COOP(CONFIG_NUM_COOP_PRIORITIES - 1), 0, -1);

void start_wifi_thread(void)
{

	while (1) {

		int ret;

		LOG_INF("Waiting for Wi-Fi to be ready");
		ret = k_sem_take(&wifi_ready_state_changed_sem, K_FOREVER);
		if (ret) {
			LOG_ERR("Failed to take semaphore: %d", ret);
			return;
		}

check_wifi_ready:
		if (!wifi_ready_status) {
			LOG_INF("Wi-Fi is not ready");
			/* Perform any cleanup and stop using Wi-Fi and wait for
			 * Wi-Fi to be ready
			 */
			continue;
		}


		wifi_connect();

		while (!context.connect_result) {
			cmd_wifi_status();
			k_sleep(K_MSEC(STATUS_POLLING_MS));
		}

		if (context.connected) {
			cmd_wifi_status();
			ret = k_sem_take(&wifi_ready_state_changed_sem, K_FOREVER);
			if (ret) {
				LOG_ERR("Failed to take semaphore: %d", ret);
				return;
			}
			goto check_wifi_ready;
		}
	}
	return;
}

void wifi_ready_cb(bool wifi_ready)
{
	LOG_DBG("Is Wi-Fi ready?: %s", wifi_ready ? "yes" : "no");
	wifi_ready_status = wifi_ready;
	k_sem_give(&wifi_ready_state_changed_sem);
}

static int register_wifi_ready(void)
{
	int ret = 0;
	wifi_ready_callback_t cb;
	struct net_if *iface = net_if_get_first_wifi();

	if (!iface) {
		LOG_ERR("Failed to get Wi-Fi interface");
		return -1;
	}

	cb.wifi_ready_cb = wifi_ready_cb;

	LOG_DBG("Registering Wi-Fi ready callbacks");
	ret = register_wifi_ready_callback(cb, iface);
	if (ret) {
		LOG_ERR("Failed to register Wi-Fi ready callbacks %s", strerror(ret));
		return ret;
	}

	return ret;
}




int wifi_init(){

	int ret = 0;

	net_mgmt_callback_init();

	ret = register_wifi_ready();
	if (ret) {
		return ret;
	}
	k_thread_start(start_wifi_thread_id);
	
	return 0;
}








/*
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

*/



