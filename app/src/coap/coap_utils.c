#include "coap_utils.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/net/socket.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/net/coap.h>
#include <zephyr/random/random.h>

LOG_MODULE_REGISTER(coap_utils, CONFIG_MY_COAP_LOG_LEVEL);

#define COAP_CLIENT_POOL_SIZE 100

struct request_entry {
	uint8_t token[TOKEN_LEN];
	uint32_t timestamp;
	uint8_t used;
};

static struct request_entry pending_requests_pool[COAP_CLIENT_POOL_SIZE];

static uint32_t timeout;

static K_MUTEX_DEFINE(pool_mutex);

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


void random_token(uint8_t *token)
{
	for (int i = 0; i < TOKEN_LEN; i++) {
		token[i] = sys_rand32_get();
	}
}



void init_pending_request_pool(uint32_t max_timeout)
{
	memset(pending_requests_pool, 0, sizeof(pending_requests_pool));
	timeout = max_timeout;
}


void remove_timedout_requests()
{
	uint32_t current_time = k_uptime_get_32();
	for (int i = 0; i < COAP_CLIENT_POOL_SIZE; i++)
	{
		if (pending_requests_pool[i].used == 1 && current_time - pending_requests_pool[i].timestamp > timeout)
		{
			LOG_WRN("Request timed out, removing token from pending request pool");
			pending_requests_pool[i].used = 0;
		}
	}
}



int add_pending_request(uint8_t * token)
{
	k_mutex_lock(&pool_mutex, K_FOREVER);
	remove_timedout_requests();
	for (int i = 0; i < COAP_CLIENT_POOL_SIZE; i++)
	{
		if (pending_requests_pool[i].used == 0)
		{
			memcpy(pending_requests_pool[i].token, token, TOKEN_LEN);
			pending_requests_pool[i].timestamp = k_uptime_get_32();
			pending_requests_pool[i].used = 1;
			k_mutex_unlock(&pool_mutex);
			return 0;
		}
	}
	LOG_ERR("No available slots in pending request pool");
	k_mutex_unlock(&pool_mutex);
	return -1;
}

uint32_t remove_pending_request(uint8_t * token)
{
	k_mutex_lock(&pool_mutex, K_FOREVER);
	remove_timedout_requests();
	for (int i = 0; i < COAP_CLIENT_POOL_SIZE; i++)
	{
		if (pending_requests_pool[i].used == 1 && memcmp(pending_requests_pool[i].token, token, TOKEN_LEN) == 0)
		{
			pending_requests_pool[i].used = 0;
			k_mutex_unlock(&pool_mutex);
			return k_uptime_get_32() - pending_requests_pool[i].timestamp;
		}
	}
	LOG_ERR("Token not found in pending request pool");
	k_mutex_unlock(&pool_mutex);
	return 0;
}
