#include "coap.h"

LOG_MODULE_REGISTER(coap, CONFIG_MY_COAP_LOG_LEVEL);

#include <zephyr/net/socket.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/net/socket.h>
#include <zephyr/random/random.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/net/tls_credentials.h>

#define APP_COAP_VERSION 1
#define APP_COAP_MAX_MSG_LEN 1280

static uint8_t coap_buf[APP_COAP_MAX_MSG_LEN];

static int sock;


static struct sockaddr_in server = { 0 };

static uint16_t next_token;

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



int coap_init()
{
    int err;

	server_resolve();

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		LOG_ERR("Failed to create CoAP socket: %d.\n", errno);
		return -errno;
	}

	err = connect(sock, (struct sockaddr *)&server, sizeof(struct sockaddr));
	if (err < 0) {
		LOG_ERR("Connect failed : %d\n", errno);
		return -errno;
	}
	LOG_INF("Successfully connected to server");

    return 0;
}

/*

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

    // send request
    int err = coap_client_req(&coap_client, sock, (struct sockaddr *)&server, &req, NULL);
    if (err) {
        LOG_ERR("Failed to send request: %d", err);
        return err;
    }

    LOG_INF("CoAP PUT request sent sent to %s, resource: %s",_host, resource);
    return 0;
}
*/
int coap_get(const char *resource)
{
    // Create the CoAP message
	struct coap_packet request;

	next_token = sys_rand32_get();

	int err = coap_packet_init(&request, coap_buf, sizeof(coap_buf),
			       APP_COAP_VERSION, COAP_TYPE_NON_CON,
			       sizeof(next_token), (uint8_t *)&next_token,
			       COAP_METHOD_GET, coap_next_id());
	if (err < 0) {
		LOG_ERR("Failed to create CoAP request, %d\n", err);
		return err;
	}

	/* STEP 7.2 - Add an option specifying the resource path */
	err = coap_packet_append_option(&request, COAP_OPTION_URI_PATH,
					(uint8_t *)CONFIG_COAP_RX_RESOURCE,
					strlen(CONFIG_COAP_RX_RESOURCE));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d\n", err);
		return err;
	}

	/* STEP 7.3 - Send the configured CoAP packet */
	err = send(sock, request.data, request.offset, 0);
	if (err < 0) {
		LOG_ERR("Failed to send CoAP request, %d\n", errno);
		return -errno;
	}

	LOG_INF("CoAP GET request sent: Token 0x%04x\n", next_token);

	return 0;
}

