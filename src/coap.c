#include "coap.h"

#include <zephyr/net/socket.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/net/socket.h>
#include <zephyr/random/random.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/net/coap.h>
#include <zephyr/net/coap_client.h>

LOG_MODULE_REGISTER(coap, CONFIG_MY_COAP_LOG_LEVEL);


static int sock;

static struct sockaddr_in server = { 0 };
static struct coap_client coap_client = { 0 };


int server_resolve()
{
    int err;

    LOG_DBG("Resolving server name: %s", CONFIG_COAP_SAMPLE_SERVER_HOSTNAME);

    // Check if the hostname is an IP address
	err = inet_pton(AF_INET, CONFIG_COAP_SAMPLE_SERVER_HOSTNAME, &server.sin_addr);
    if (err == 1) {
        // Successfully converted IP address
        server.sin_family = AF_INET;
		server.sin_port = htons(CONFIG_COAP_SAMPLE_SERVER_PORT);
        LOG_DBG("Server resolved: %s", CONFIG_COAP_SAMPLE_SERVER_HOSTNAME);
        return 0;
    } else if (err == 0) {
        // Not a valid IP address, proceed with DNS resolution
		struct addrinfo hints = {
			.ai_family = AF_INET,
			.ai_socktype = SOCK_DGRAM
		};
		struct addrinfo *result;
		int addr_err;
		char ipv4_addr[NET_IPV4_ADDR_LEN];

		addr_err = getaddrinfo(CONFIG_COAP_SAMPLE_SERVER_HOSTNAME, NULL, &hints, &result);
		if (addr_err != 0) {
			LOG_ERR("getaddrinfo failed, error: %d", addr_err);
			return -EIO;
		}

		if (result == NULL) {
			LOG_ERR("Address not found");
			return -ENOENT;
		}

		server.sin_addr.s_addr =((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
		server.sin_family = AF_INET;
		server.sin_port = htons(CONFIG_COAP_SAMPLE_SERVER_PORT);

		inet_ntop(AF_INET, &server.sin_addr.s_addr, ipv4_addr,sizeof(ipv4_addr));
		LOG_INF("IPv4 Address found %s", ipv4_addr);

		//free result pointer
		freeaddrinfo(result);

        return 0;
    } else {
        // inet_pton failed
        LOG_ERR("inet_pton failed, error: %d", errno);
        return -errno;
    }
}


static void response_cb(int16_t code, size_t offset, const uint8_t *payload,
			size_t len, bool last_block, void *user_data)
{
	if (code >= 0) {
		if(len==0){
			LOG_INF("CoAP response: code: 0x%x", code);
		}else{
			LOG_INF("CoAP response: code: 0x%x, payload: %s", code, payload);
		}
	} else {
		LOG_INF("Response received with error code: %d", code);
	}
}


int coap_init(const char* host,int port)
{
    int err;

    err = server_resolve();
	if (err) {
		LOG_ERR("Failed to resolve server name");
		return err;
	}

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		LOG_ERR("Failed to create CoAP socket: %d.", -errno);
		return -errno;
	}

	LOG_INF("Initializing CoAP client");

	err = coap_client_init(&coap_client, NULL);
	if (err) {
		LOG_ERR("Failed to initialize CoAP client: %d", err);
		return err;
	}
    return 0;
}



int coap_put(const char *resource,uint8_t *payload,size_t len)
{
    struct coap_client_request req = {
		.method = COAP_METHOD_PUT,
		.confirmable = true,
		.fmt = COAP_CONTENT_FORMAT_TEXT_PLAIN,
		.payload = payload,
		.cb = response_cb,
		.len = len,
		.path = resource,
	};

    /* Send request */
    int err = coap_client_req(&coap_client, sock, (struct sockaddr *)&server, &req, NULL);
    if (err) {
        LOG_ERR("Failed to send request: %d", err);
        return err;
    }

    LOG_INF("CoAP PUT request sent sent to %s, resource: %s",CONFIG_COAP_SAMPLE_SERVER_HOSTNAME, resource);
    return 0;
}

int coap_get(const char *resource)
{
    struct coap_client_request req = {
		.method = COAP_METHOD_GET,
		.confirmable = true,
		.fmt = COAP_CONTENT_FORMAT_TEXT_PLAIN,
		.payload = NULL,
		.cb = response_cb,
		.len = 0,
		.path = resource,
	};

    /* Send request */
    int err = coap_client_req(&coap_client, sock, (struct sockaddr *)&server, &req, NULL);
    if (err) {
        LOG_ERR("Failed to send request: %d", err);
        return err;
    }

    LOG_INF("CoAP GET request sent sent to %s, resource: %s",CONFIG_COAP_SAMPLE_SERVER_HOSTNAME, resource);
    return 0;
}

int coap_observe(const char *resource, bool start_observe)
{
    struct coap_client_option observe_option = {
        .code = COAP_OPTION_OBSERVE,            
        .len = 1,             
        .value = { start_observe ? 0 : 1 }, 
    };

    struct coap_client_request req = {
        .method = COAP_METHOD_GET,
        .confirmable = true,
        .fmt = COAP_CONTENT_FORMAT_TEXT_PLAIN,
        .payload = NULL,
        .len = 0,
        .cb = response_cb,
        .path = resource,
        .options = &observe_option,
        .num_options = 1,
    };

    int err = coap_client_req(&coap_client, sock, (struct sockaddr *)&server, &req, NULL);
    if (err) {
        LOG_ERR("Failed to send CoAP Observe request: %d", err);
        return err;
    }

    LOG_INF("CoAP Observe %s request sent to %s, resource: %s",
            start_observe ? "start" : "stop", CONFIG_COAP_SAMPLE_SERVER_HOSTNAME, resource);
    return 0;
}