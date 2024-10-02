#include "coap.h"

LOG_MODULE_REGISTER(coap, CONFIG_MY_COAP_LOG_LEVEL);

static int sock;
static struct sockaddr_storage server = { 0 };
static struct coap_client coap_client = { 0 };

char * _host;

int server_resolve(struct sockaddr_storage *server,const char * hostname,int port)
{
    _host=k_malloc(strlen(hostname));
    strcpy(_host,hostname);
	int err;
	struct addrinfo *result;
	struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_socktype = SOCK_DGRAM
	};
	char ipv4_addr[NET_IPV4_ADDR_LEN];

	err = getaddrinfo(hostname, NULL, &hints, &result);
	if (err) {
		LOG_ERR("getaddrinfo, error: %d", err);
		return err;
	}

	if (result == NULL) {
		LOG_ERR("Address not found");
		return -ENOENT;
	}

	/* IPv4 Address. */
	struct sockaddr_in *server4 = ((struct sockaddr_in *)server);

	server4->sin_addr.s_addr = ((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
	server4->sin_family = AF_INET;
	server4->sin_port = htons(port);

	inet_ntop(AF_INET, &server4->sin_addr.s_addr, ipv4_addr, sizeof(ipv4_addr));

	LOG_INF("IPv4 Address found %s", ipv4_addr);

	/* Free the address. */
	freeaddrinfo(result);

	return 0;
}


void response_cb(int16_t code, size_t offset, const uint8_t *payload,
			size_t len, bool last_block, void *user_data)
{
	if (code >= 0) {
		LOG_INF("CoAP response: code: 0x%x, payload: %s", code, payload);
	} else {
		LOG_INF("Response received with error code: %d", code);
	}
}


int coap_init(const char* host,int port)
{
    int err;

    err = server_resolve(&server,host,port);
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

    /* Send request */
    int err = coap_client_req(&coap_client, sock, (struct sockaddr *)&server, &req, NULL);
    if (err) {
        LOG_ERR("Failed to send request: %d", err);
        return err;
    }

    LOG_INF("CoAP GET request sent sent to %s, resource: %s",_host, resource);
    return 0;
}