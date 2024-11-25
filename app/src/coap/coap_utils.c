#include "coap_utils.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/net/socket.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/net/coap.h>
#include <zephyr/net/coap_client.h>
#include <zephyr/random/rand32.h>

LOG_MODULE_REGISTER(coap_utils, CONFIG_MY_COAP_LOG_LEVEL);

#define COAP_REQUESTS_HEAP_SIZE 32768  //increase heap size if using large payloads
static K_HEAP_DEFINE(coap_requests_heap, COAP_REQUESTS_HEAP_SIZE);

#ifndef CONFIG_IP_PROTO_IPV6
int server_resolve(struct sockaddr_in* server_ptr)
{
    int err;

    LOG_DBG("Resolving server name: %s", CONFIG_COAP_TEST_SERVER_HOSTNAME);

    // Check if the hostname is an IP address
	err = inet_pton(AF_INET, CONFIG_COAP_TEST_SERVER_HOSTNAME, &server_ptr->sin_addr);
    if (err == 1) {
        // Successfully converted IP address
        server_ptr->sin_family = AF_INET;
		server_ptr->sin_port = htons(CONFIG_COAP_TEST_SERVER_PORT);
        LOG_DBG("Server resolved: %s", CONFIG_COAP_TEST_SERVER_HOSTNAME);
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

		addr_err = getaddrinfo(CONFIG_COAP_TEST_SERVER_HOSTNAME, NULL, &hints, &result);
		if (addr_err != 0) {
			LOG_ERR("getaddrinfo failed, error: %d", addr_err);
			return -EIO;
		}

		if (result == NULL) {
			LOG_ERR("Address not found");
			return -ENOENT;
		}

		server_ptr->sin_addr.s_addr =((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
		server_ptr->sin_family = AF_INET;
		server_ptr->sin_port = htons(CONFIG_COAP_TEST_SERVER_PORT);

		inet_ntop(AF_INET, &server_ptr->sin_addr.s_addr, ipv4_addr,sizeof(ipv4_addr));
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
#else // CONFIG_IP_PROTO_IPV6
int server_resolve(struct sockaddr_in6* server_ptr)
{
	int err;

	LOG_DBG("Resolving server name: %s", CONFIG_COAP_TEST_SERVER_HOSTNAME);

	// Check if the hostname is an IPv6 address
	err = inet_pton(AF_INET6, CONFIG_COAP_TEST_SERVER_HOSTNAME, &server_ptr->sin6_addr);
	if (err == 1) {
		// Successfully converted IP address
		server_ptr->sin6_family = AF_INET6;
		server_ptr->sin6_port = htons(CONFIG_COAP_TEST_SERVER_PORT);
		LOG_DBG("Server resolved: %s", CONFIG_COAP_TEST_SERVER_HOSTNAME);
		return 0;
	} else if (err == 0) {
		// Not a valid IP address
		struct addrinfo hints = {
			.ai_family = AF_INET6,
			.ai_socktype = SOCK_DGRAM
		};
		struct addrinfo *result;
		int addr_err;
		char ipv6_addr[INET6_ADDRSTRLEN];

		addr_err = getaddrinfo(CONFIG_COAP_TEST_SERVER_HOSTNAME, NULL, &hints, &result);
		if (addr_err != 0) {
			LOG_ERR("getaddrinfo failed, error: %d", addr_err);
			return -EIO;
		}

		if (result == NULL) {
			LOG_ERR("Address not found");
			return -ENOENT;
		}

		server_ptr->sin6_addr = ((struct sockaddr_in6 *)result->ai_addr)->sin6_addr;
		server_ptr->sin6_family = AF_INET6;
		server_ptr->sin6_port = htons(CONFIG_COAP_TEST_SERVER_PORT);

		inet_ntop(AF_INET6, &server_ptr->sin6_addr, ipv6_addr, sizeof(ipv6_addr));
		LOG_INF("IPv6 Address found %s", ipv6_addr);

		// Free result pointer
		freeaddrinfo(result);

		return 0;
	} else {
		// inet_pton failed
		LOG_ERR("inet_pton failed, error: %d", errno);
		return -errno;
	}
}
#endif // CONFIG_IP_PROTO_IPV6


void random_token(uint8_t *token, uint8_t token_len)
{
	for (int i = 0; i < token_len; i++) {
		token[i] = sys_rand32_get();
	}
}
