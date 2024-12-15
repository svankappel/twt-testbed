

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/random/random.h>

#include "wifi_sta.h"
#include "wifi_ps.h"
#include "wifi_twt.h"


#include <zephyr/net/lwm2m.h>



LOG_MODULE_REGISTER(main, CONFIG_MY_MAIN_LOG_LEVEL);


static int device_reboot_cb(uint16_t obj_inst_id, uint8_t *args,
                            uint16_t args_len)
{
        LOG_INF("Reboot CB called !");
        
        return 0; /* won't reach this */
}

static void rd_client_event(struct lwm2m_ctx *client,
                            enum lwm2m_rd_client_event client_event)
{
        switch (client_event) {

        case LWM2M_RD_CLIENT_EVENT_NONE:
                /* do nothing */
                break;

        case LWM2M_RD_CLIENT_EVENT_BOOTSTRAP_REG_FAILURE:
                LOG_DBG("Bootstrap registration failure!");
                break;

        case LWM2M_RD_CLIENT_EVENT_BOOTSTRAP_REG_COMPLETE:
                LOG_DBG("Bootstrap registration complete");
                break;

        case LWM2M_RD_CLIENT_EVENT_BOOTSTRAP_TRANSFER_COMPLETE:
                LOG_DBG("Bootstrap transfer complete");
                break;

        case LWM2M_RD_CLIENT_EVENT_REGISTRATION_FAILURE:
                LOG_DBG("Registration failure!");
                break;

        case LWM2M_RD_CLIENT_EVENT_REGISTRATION_COMPLETE:
                LOG_DBG("Registration complete");
                break;

        case LWM2M_RD_CLIENT_EVENT_REG_TIMEOUT:
                LOG_DBG("Registration timeout!");
                break;

        case LWM2M_RD_CLIENT_EVENT_REG_UPDATE_COMPLETE:
                LOG_DBG("Registration update complete");
                break;

        case LWM2M_RD_CLIENT_EVENT_DEREGISTER_FAILURE:
                LOG_DBG("Deregister failure!");
                break;

        case LWM2M_RD_CLIENT_EVENT_DISCONNECT:
                LOG_DBG("Disconnected");
                break;

        case LWM2M_RD_CLIENT_EVENT_REG_UPDATE:
                LOG_DBG("Registration update");
                break;

        case LWM2M_RD_CLIENT_EVENT_DEREGISTER:
                LOG_DBG("Deregistration client");
                break;

        case LWM2M_RD_CLIENT_EVENT_SERVER_DISABLED:
                LOG_DBG("LwM2M server disabled");
          break;
        }
}

static void observe_cb(enum lwm2m_observe_event event,
		       struct lwm2m_obj_path *path, void *user_data)
{
	char buf[LWM2M_MAX_PATH_STR_SIZE];

	switch (event) {

	case LWM2M_OBSERVE_EVENT_OBSERVER_ADDED:
		LOG_INF("Observer added for %s", lwm2m_path_log_buf(buf, path));
		break;

	case LWM2M_OBSERVE_EVENT_OBSERVER_REMOVED:
		LOG_INF("Observer removed for %s", lwm2m_path_log_buf(buf, path));
		break;

	case LWM2M_OBSERVE_EVENT_NOTIFY_ACK:
		LOG_INF("Notify acknowledged for %s", lwm2m_path_log_buf(buf, path));
		break;

	case LWM2M_OBSERVE_EVENT_NOTIFY_TIMEOUT:
		LOG_INF("Notify timeout for %s, trying registration update",
			lwm2m_path_log_buf(buf, path));

		lwm2m_rd_client_update();
		break;
	}
}


int main(void)
{
    int ret;

    LOG_INF("Starting TWT testbed ...");

    // initialize setup

    wifi_init();

    ret = wifi_connect();
    if(ret != 0)
    {
        LOG_ERR("Failed to connect to wifi");
        k_sleep(K_FOREVER);
    }

    /* LwM2M client context */
    static struct lwm2m_ctx client;

    lwm2m_set_string(&LWM2M_OBJ(0, 0, 0), "coap://leshan.eclipseprojects.io:5683");

    //nosec
    lwm2m_set_u8(&LWM2M_OBJ(0, 0, 2), 3);

    //bootstrap
    lwm2m_set_u8(&LWM2M_OBJ(0, 0, 1), 0); // Disable bootstrap


    // short server id
    lwm2m_set_u16(&LWM2M_OBJ(0, 0, 10), CONFIG_LWM2M_SERVER_DEFAULT_SSID);
	lwm2m_set_u16(&LWM2M_OBJ(1, 0, 0), CONFIG_LWM2M_SERVER_DEFAULT_SSID);


    //client manufacturer
    lwm2m_set_res_buf(&LWM2M_OBJ(3, 0, 0), "CLIENT_MANUFACTURER",
                   sizeof("CLIENT_MANUFACTURER"),
                   LWM2M_RES_DATA_FLAG_RO,LWM2M_RES_DATA_FLAG_RO);
    

    lwm2m_register_exec_callback(&LWM2M_OBJ(3, 0, 4), device_reboot_cb);

    (void)memset(&client, 0x0, sizeof(client));

    lwm2m_rd_client_start(&client, "svk-unique-endpoint-name", 0, rd_client_event,observe_cb);


    k_sleep(K_FOREVER);
    return 0;
}
