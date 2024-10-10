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

LOG_MODULE_REGISTER(coap, CONFIG_MY_COAP_LOG_LEVEL);

#define APP_COAP_VERSION 1
#define APP_COAP_MAX_MSG_LEN 1280

static uint8_t coap_buf[APP_COAP_MAX_MSG_LEN];

static int sock;

static struct sockaddr_in server = { 0 };

static uint16_t next_token;

#define COAP_CLIENT_THREAD_STACK_SIZE 2048
#define COAP_CLIENT_THREAD_PRIORITY K_PRIO_PREEMPT(7)

K_THREAD_STACK_DEFINE(COAP_THREAD_STACK, COAP_CLIENT_THREAD_STACK_SIZE);

static struct k_thread coap_client_thread;

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

static int client_handle_response(uint8_t *buf, int received)
{
	struct coap_packet reply;
	uint8_t token[8];
	uint16_t token_len;
	const uint8_t *payload;
	uint16_t payload_len;
	uint8_t temp_buf[128];

	// Parse the received CoAP packet
	int err = coap_packet_parse(&reply, buf, received, NULL, 0);
	if (err < 0) {
		LOG_ERR("Malformed response received: %d\n", err);
		return err;
	}

	// Confirm the token in the response matches the token sent
	token_len = coap_header_get_token(&reply, token);
	if ((token_len != sizeof(next_token)) ||
	    (memcmp(&next_token, token, sizeof(next_token)) != 0)) {
		LOG_ERR("Invalid token received: 0x%02x%02x\n",
		       token[1], token[0]);
		return 0;
	}

	// Retrieve the payload and confirm it's nonzero
	payload = coap_packet_get_payload(&reply, &payload_len);

	if (payload_len > 0) {
		snprintf(temp_buf, MIN(payload_len + 1, sizeof(temp_buf)), "%s", payload);
	} else {
		strcpy(temp_buf, "EMPTY");
	}

	// Log the header code, token and payload of the response
	LOG_INF("CoAP response: Code 0x%x, Token 0x%02x%02x, Payload: %s",
	       coap_header_get_code(&reply), token[1], token[0], (char *)temp_buf);

	return 0;
}

void coap_thread(void)
{
	int received;
	int err;

	while (1) {
		// Receive response from the CoAP server
		received = recv(sock, coap_buf, sizeof(coap_buf), 0);

		if (received < 0) {
			LOG_ERR("Socket error: %d, exit", errno);
			break;
		} else if (received == 0) {
			LOG_INF("Empty datagram");
			continue;
		}

		// Parse the received CoAP packet
		err = client_handle_response(coap_buf, received);
		if (err < 0) {
			LOG_ERR("Invalid response, exit");
			break;
		}
	}
}



int coap_init()
{
    int err;

	server_resolve();

	k_sleep(K_MSEC(500));

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


	k_thread_create(&coap_client_thread, COAP_THREAD_STACK,
					 K_THREAD_STACK_SIZEOF(COAP_THREAD_STACK),
					 (k_thread_entry_t)coap_thread,
					 NULL, NULL, NULL,
					 COAP_CLIENT_THREAD_PRIORITY, 0, K_NO_WAIT);

	k_thread_name_set(&coap_client_thread, "coap_client_thread");

	k_thread_start(&coap_client_thread);

    return 0;
}



int coap_put(const char *resource,uint8_t *payload,size_t len)
{
    int err;
	struct coap_packet request;

	next_token = sys_rand32_get();

	// Initialize the CoAP packet and append the resource path
	err = coap_packet_init(&request, coap_buf, sizeof(coap_buf),
			       APP_COAP_VERSION, COAP_TYPE_NON_CON,
			       sizeof(next_token), (uint8_t *)&next_token,
			       COAP_METHOD_PUT, coap_next_id());
	if (err < 0) {
		LOG_ERR("Failed to create CoAP request, %d\n", err);
		return err;
	}

	err = coap_packet_append_option(&request, COAP_OPTION_URI_PATH,
					(uint8_t *)resource,
					strlen(resource));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d\n", err);
		return err;
	}

	// Append the content format as plain text
	const uint8_t text_plain = COAP_CONTENT_FORMAT_TEXT_PLAIN;
	err = coap_packet_append_option(&request, COAP_OPTION_CONTENT_FORMAT,
					&text_plain,
					sizeof(text_plain));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d\n", err);
		return err;
	}

	// Add the payload to the message
	err = coap_packet_append_payload_marker(&request);
	if (err < 0) {
		LOG_ERR("Failed to append payload marker, %d\n", err);
		return err;
	}

	err = coap_packet_append_payload(&request, payload, len);
	if (err < 0) {
		LOG_ERR("Failed to append payload, %d\n", err);
		return err;
	}

	err = send(sock, request.data, request.offset, 0);
	if (err < 0) {
		LOG_ERR("Failed to send CoAP request, %d\n", errno);
		return -errno;
	}

	LOG_INF("CoAP PUT request sent: Token 0x%04x", next_token);

	return 0;
}

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
		LOG_ERR("Failed to create CoAP request, %d", err);
		return err;
	}

	// Add an option specifying the resource path 
	err = coap_packet_append_option(&request, COAP_OPTION_URI_PATH,
									(uint8_t *)resource,
									strlen(resource));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d", err);
		return err;
	}

	// Send the configured CoAP packet 
	err = send(sock, request.data, request.offset, 0);
	if (err < 0) {
		LOG_ERR("Failed to send CoAP request, %d", errno);
		return -errno;
	}

	LOG_INF("CoAP GET request sent: Token 0x%04x", next_token);

	return 0;
}

