/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/random/random.h>

#include <zephyr/net/lwm2m.h>
#include "lwm2m_resource_ids.h"

#include "wifi/wifi_sta.h"
//#include "coap.h"
#include "profiler.h"

LOG_MODULE_REGISTER(main, CONFIG_MY_MAIN_LOG_LEVEL);

#define CLIENT_MANUFACTURER "Zephyr"

/* LwM2M client context */
static struct lwm2m_ctx client;

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

static void socket_state(int fd, enum lwm2m_socket_states state)
{
	(void) fd;
	switch (state) {
	case LWM2M_SOCKET_STATE_ONGOING:
		LOG_DBG("LWM2M_SOCKET_STATE_ONGOING");
		break;
	case LWM2M_SOCKET_STATE_ONE_RESPONSE:
		LOG_DBG("LWM2M_SOCKET_STATE_ONE_RESPONSE");
		break;
	case LWM2M_SOCKET_STATE_LAST:
		LOG_DBG("LWM2M_SOCKET_STATE_LAST");
		break;
	case LWM2M_SOCKET_STATE_NO_DATA:
		LOG_DBG("LWM2M_SOCKET_STATE_NO_DATA");
		break;
	}
}

static int lwm2m_setup(void)
{
        int err;
        /* Save power by not updating timestamp on device object */
	//lwm2m_update_device_service_period(0);

        /* Set the server URI */
        lwm2m_set_string(&LWM2M_OBJ(0, 0, 0), "coap://192.168.50.206:5683");
        
        /* Set security mode (no security for this example) */
        lwm2m_set_u8(&LWM2M_OBJ(0, 0, 2), 3); // No security

        lwm2m_set_u16(&LWM2M_OBJ(0, 0, 10), CONFIG_LWM2M_SERVER_DEFAULT_SSID);
        lwm2m_set_u16(&LWM2M_OBJ(1, 0, 0), CONFIG_LWM2M_SERVER_DEFAULT_SSID);

        lwm2m_set_res_buf(&LWM2M_OBJ(3,0,0),CLIENT_MANUFACTURER,sizeof(CLIENT_MANUFACTURER),sizeof(CLIENT_MANUFACTURER),LWM2M_RES_DATA_FLAG_RO);

        /* Set up client registration */
        (void)memset(&client, 0x0, sizeof(client));
        client.set_socket_state = socket_state;
        err = lwm2m_rd_client_start(&client, "my_device_nrf70", 0, rd_client_event,NULL);
        if (err) {
                LOG_ERR("Failed to start LwM2M client");
                return err;
        }


        return 0;
}



int main(void)
{
	profiler_init();
	wifi_init();
	wifi_connect();

	k_sleep(K_SECONDS(1));

	// Initialize LwM2M client
    
    int err = lwm2m_setup();
    if (err) {
        LOG_ERR("LwM2M setup failed: %d", err);
        return;
    }

    /* Main loop */
    while (1) {
        k_sleep(K_SECONDS(10)); // Sleep to save power
    }

	return 0;
}
