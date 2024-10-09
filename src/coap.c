#include "coap.h"

LOG_MODULE_REGISTER(coap, CONFIG_MY_COAP_LOG_LEVEL);

static int sock;


static struct sockaddr_in server = { 0 };

char * _host;


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
		struct addrinfo *res;
		int addr_err;

		addr_err = getaddrinfo(CONFIG_COAP_SAMPLE_SERVER_HOSTNAME, NULL, &hints, &result);
		if (addr_err != 0) {
			LOG_ERR("getaddrinfo failed, error: %d", addr_err);
			return -EIO;
		}

		for (res = result; res != NULL; res = res->ai_next) {
			if (res->ai_family == AF_INET) {
				struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
				server.sin_family = AF_INET;
				server.sin_port = htons(CONFIG_COAP_SAMPLE_SERVER_PORT);
				server.sin_addr = addr->sin_addr;
				break;
			}
		}

		freeaddrinfo(result);
        LOG_DBG("Server resolved: %s", hostname);
        return 0;
		*/
    } else {
        // inet_pton failed
        LOG_ERR("inet_pton failed, error: %d", errno);
        return -errno;
    }
}



int coap_init(const char* host,int port)
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

    //send request
    int err = coap_client_req(&coap_client, sock, (struct sockaddr *)&server, &req, NULL);
    if (err) {
        LOG_ERR("Failed to send request: %d", err);
        return err;
    }

    LOG_INF("CoAP GET request sent sent to %s, resource: %s",_host, resource);
    return 0;
}

*/