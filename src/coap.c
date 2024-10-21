#include "coap.h"

#include <zephyr/net/socket.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/net/socket.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/net/coap.h>
#include <zephyr/net/coap_client.h>

LOG_MODULE_REGISTER(coap, CONFIG_MY_COAP_LOG_LEVEL);

#define STACK_SIZE 4096
#define PRIORITY -8 

K_THREAD_STACK_DEFINE(thread_stack, STACK_SIZE);
struct k_thread coap_thread_data;

K_SEM_DEFINE(send_sem, 0, 1);
K_SEM_DEFINE(next_sem,1,1);

K_SEM_DEFINE(init_sem, 0, 1);



struct coap_client_request* req_ptr;
struct coap_transmission_parameters* req_params_ptr;


int server_resolve(struct sockaddr_in* server)
{
    int err;

    LOG_DBG("Resolving server name: %s", CONFIG_COAP_SAMPLE_SERVER_HOSTNAME);

    // Check if the hostname is an IP address
	err = inet_pton(AF_INET, CONFIG_COAP_SAMPLE_SERVER_HOSTNAME, &server->sin_addr);
    if (err == 1) {
        // Successfully converted IP address
        server->sin_family = AF_INET;
		server->sin_port = htons(CONFIG_COAP_SAMPLE_SERVER_PORT);
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

		server->sin_addr.s_addr =((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
		server->sin_family = AF_INET;
		server->sin_port = htons(CONFIG_COAP_SAMPLE_SERVER_PORT);

		inet_ntop(AF_INET, &server->sin_addr.s_addr, ipv4_addr,sizeof(ipv4_addr));
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


void coap_thread(void *arg1, void *arg2, void *arg3)
{
	int sock;
	struct sockaddr_in server = { 0 };
	struct coap_client coap_client = { 0 };

	struct coap_client_request req = { 0 };
	req_ptr = &req;

	struct coap_transmission_parameters req_params = { 0 };
	req_params_ptr = &req_params;

	//initialize request
	req.confirmable = true;
	req.fmt = COAP_CONTENT_FORMAT_TEXT_PLAIN;
	req.cb = response_cb;
	req.num_options = 0;
	req.options = NULL;

	req_params.coap_backoff_percent=100;
	req_params.max_retransmission=0;

	//initialize server

	int err;

    err = server_resolve(&server);
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

	k_sem_give(&init_sem);

	while (1) {
		//wait for semaphore
		k_sem_take(&send_sem, K_FOREVER);

		/* Send request */
		int err = coap_client_req(&coap_client, sock, (struct sockaddr *)&server, &req, &req_params);
		if (err) {
			LOG_ERR("Failed to send request: %d", err);
			continue;
		}

		if(req.path!=NULL){
			k_free(req.path);
		}

		if(req.payload!=NULL){
			k_free(req.payload);
		}

		LOG_INF("CoAP PUT request sent to %s, resource: %s",CONFIG_COAP_SAMPLE_SERVER_HOSTNAME, req.path);

		k_sem_give(&next_sem);
	}
}



int coap_put(const char *resource,uint8_t *payload, uint32_t timeout)
{
	k_sem_take(&next_sem, K_FOREVER);

	req_ptr->method = COAP_METHOD_PUT;

	req_ptr->path = k_malloc(strlen(resource) + 1);
	if (req_ptr->path == NULL) {
		LOG_ERR("Failed to allocate memory");
		return -ENOMEM;
	}
	strcpy(req_ptr->path,resource);

	req_ptr->payload = k_malloc(strlen(payload) + 1); 
	if (req_ptr->payload == NULL) {
		LOG_ERR("Failed to allocate memory");
		return -ENOMEM;
	}
	strcpy(req_ptr->payload, payload);

	req_ptr->len = strlen(payload);

	req_params_ptr->ack_timeout=timeout;

	k_sem_give(&send_sem);
}

int coap_get(const char *resource, uint32_t timeout)
{
	k_sem_take(&next_sem, K_FOREVER);

	req_ptr->method = COAP_METHOD_GET;
	req_ptr->payload = NULL;
	req_ptr->len = 0;
	
	req_ptr->path = k_malloc(strlen(resource) + 1);
	if (req_ptr->path == NULL) {
		LOG_ERR("Failed to allocate memory");
		return -ENOMEM;
	}
	strcpy(req_ptr->path,resource);

	req_params_ptr->ack_timeout = timeout;

	k_sem_give(&send_sem);

	return 0;
}

/*
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
*/

// Function to initialize the test
int coap_init() {
	
	//start thread to handle CoAP 

    k_tid_t thread_id = k_thread_create(&coap_thread_data, thread_stack,
                                        K_THREAD_STACK_SIZEOF(thread_stack),
                                        coap_thread,
                                        NULL, NULL, NULL,
                                        PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(thread_id, "test_thread");
    k_thread_start(thread_id);

	k_sem_take(&init_sem, K_FOREVER);

	return 0;
}
