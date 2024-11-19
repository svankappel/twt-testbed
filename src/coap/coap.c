#include "coap.h"

#include "coap_utils.h"

#include <zephyr/net/socket.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/net/coap.h>
#include <zephyr/net/coap_client.h>

LOG_MODULE_REGISTER(coap, CONFIG_MY_COAP_LOG_LEVEL);


#define STACK_SIZE 4096
#define PRIORITY 8

static K_THREAD_STACK_DEFINE(thread_stack, STACK_SIZE);
struct k_thread coap_thread_data;

static K_SEM_DEFINE(send_sem, 0, 1);
static K_SEM_DEFINE(sent_sem, 0, 1);

static K_SEM_DEFINE(init_sem, 0, 1);
static K_SEM_DEFINE(validate_sem, 0, 1);


#define MSGQ_SIZE 1
K_MSGQ_DEFINE(coap_req_msgq, sizeof(struct coap_client_request*), MSGQ_SIZE, 4);
K_MSGQ_DEFINE(coap_ret_msgq, sizeof(int), MSGQ_SIZE, 4);
K_MSGQ_DEFINE(coap_stat_msgq, sizeof(int), MSGQ_SIZE, 4);


static void (*coap_response_callback)(int16_t code, void * user_data) = NULL;
static void * coap_response_callback_user_data = NULL;

void coap_register_response_callback(void (*callback)(int16_t code, void * user_data),void * callback_user_data) {
	coap_response_callback_user_data = callback_user_data;
	coap_response_callback = callback;
}



static void put_response_cb(int16_t code, size_t offset, const uint8_t *payload,
			size_t len, bool last_block, void *user_data)
{
	if (code >= 0) {
		if(len==0){
			LOG_INF("CoAP response: code: 0x%x", code);
		}else if(len > 50){
			char payl[51];
			strncpy(payl,(const char *)payload,50);
			payl[50]='\0';
			LOG_INF("CoAP response: code: 0x%x, payload: %s...", code, payl);

		}
		else{
			LOG_INF("CoAP response: code: 0x%x, payload: %s", code, payload);
		}
	} else {
		LOG_INF("Error received with code: %d", code);
	}
	free_coap_request(user_data);
	if(coap_response_callback)
	{
		(*coap_response_callback)(code, coap_response_callback_user_data);
	}
}

static void valid_response_cb(int16_t code, size_t offset, const uint8_t *payload,
			size_t len, bool last_block, void *user_data)
{
	if (code >= 0) {
		if(len==0){
			LOG_INF("CoAP validate response: code: 0x%x", code);
		}else{
			LOG_INF("CoAP validate response: code: 0x%x, payload: %s", code, payload);
		}
	} else {
		LOG_INF("CoAP validate request timed out: error code: %d", code);
	}
	free_coap_request(user_data);
	if (code >= 0 && len > 0) {
		if(strcmp(payload,"valid")==0){
			k_sem_give(&validate_sem);
		}
	}
}

static void stat_response_cb(int16_t code, size_t offset, const uint8_t *payload,
			size_t len, bool last_block, void *user_data)
{
	if (code >= 0) {
		if(len==0){
			LOG_INF("CoAP stat response: code: 0x%x", code);
		}else{
			LOG_INF("CoAP stat response: code: 0x%x, payload: %s", code, payload);
		}
	} else {
		LOG_INF("CoAP stat request timed out: error code: %d", code);
	}
	free_coap_request(user_data);
	if (code >= 0 && len > 0) {
		int stat_value = atoi((const char *)payload);
		if (k_msgq_put(&coap_stat_msgq, &stat_value, K_NO_WAIT) != 0) {
			LOG_ERR("Failed to enqueue CoAP stat");
		}
		
	}
}

int coap_put(char *resource,uint8_t *payload, uint32_t timeout)
{
	size_t resource_len = strlen(resource)+1;

	struct coap_client_request* req = alloc_coap_request(resource_len, strlen(payload)+1);
	struct coap_transmission_parameters* req_params = ((struct request_user_data*)req->user_data)->req_params;


	req->method = COAP_METHOD_PUT;
	req->confirmable = true;
	strncpy((char*)req->path,resource,resource_len);
	req->fmt = COAP_CONTENT_FORMAT_TEXT_PLAIN;
	req->cb = put_response_cb;
	strcpy(req->payload,payload);
	req->len = strlen(payload);
	req->num_options = 0;
	req->options = NULL;

	req_params->ack_timeout = timeout;
	req_params->coap_backoff_percent = 100;
	req_params->max_retransmission = 0;

	if (k_msgq_put(&coap_req_msgq, &(req->user_data), K_NO_WAIT) != 0) {
        LOG_ERR("Failed to enqueue CoAP request");
        free_coap_request(req->user_data);
        return -ENOMEM;
    }

	k_sem_give(&send_sem);
	k_sem_take(&sent_sem, K_FOREVER);

	int ret;

	if (k_msgq_get(&coap_ret_msgq, &ret, K_NO_WAIT) != 0) {
		LOG_ERR("Failed to retrieve return value from message queue");
		return -ENOEXEC;
	}
	return ret;
}


int coap_validate()
{
	char *resource_path = "validate";
	size_t resource_path_len = strlen(resource_path)+1;

	struct coap_client_request* req = alloc_coap_request(resource_path_len, 0);
	struct coap_transmission_parameters* req_params = ((struct request_user_data*)req->user_data)->req_params;

	req->method = COAP_METHOD_GET;
	req->confirmable = true;
	strncpy((char*)req->path,resource_path,resource_path_len);
	req->fmt = COAP_CONTENT_FORMAT_TEXT_PLAIN;
	req->cb = valid_response_cb;
	req->payload = NULL;
	req->len = 0;
	req->num_options = 0;
	req->options = NULL;

	req_params->ack_timeout = 1000;
	req_params->coap_backoff_percent = 100;
	req_params->max_retransmission = 5;

	if (k_msgq_put(&coap_req_msgq, &(req->user_data), K_NO_WAIT) != 0) {
        LOG_ERR("Failed to enqueue CoAP request");
        free_coap_request(req->user_data);
        return -ENOMEM;
    }

	k_sem_give(&send_sem);
	k_sem_take(&sent_sem, K_FOREVER);

	int ret;

	if (k_msgq_get(&coap_ret_msgq, &ret, K_NO_WAIT) != 0) {
		LOG_ERR("Failed to retrieve return value from message queue");
		return -ENOEXEC;
	}

	if (k_sem_take(&validate_sem, K_SECONDS(10)) != 0) {
		LOG_ERR("Validation timed out");
		return -ETIMEDOUT;
	}
	return ret;
}

int coap_get_stat()
{
	char *resource_path = "stat";
	size_t resource_path_len = strlen(resource_path)+1;

	struct coap_client_request* req = alloc_coap_request(resource_path_len, 0);
	struct coap_transmission_parameters* req_params = ((struct request_user_data*)req->user_data)->req_params;

	req->method = COAP_METHOD_GET;
	req->confirmable = true;
	strncpy((char*)req->path,resource_path,resource_path_len);
	req->fmt = COAP_CONTENT_FORMAT_TEXT_PLAIN;
	req->cb = stat_response_cb;
	req->payload = NULL;
	req->len = 0;
	req->num_options = 0;
	req->options = NULL;

	req_params->ack_timeout = 1000;
	req_params->coap_backoff_percent = 100;
	req_params->max_retransmission = 5;

	if (k_msgq_put(&coap_req_msgq, &(req->user_data), K_NO_WAIT) != 0) {
        LOG_ERR("Failed to enqueue CoAP request");
        return -ENOMEM;
    }

	k_sem_give(&send_sem);
	k_sem_take(&sent_sem, K_FOREVER);

	int ret;

	if (k_msgq_get(&coap_ret_msgq, &ret, K_NO_WAIT) != 0) {
		LOG_ERR("Failed to retrieve return value from message queue");
		return -ENOEXEC;
	}

	if (k_msgq_get(&coap_stat_msgq, &ret, K_SECONDS(10)) != 0) {
		LOG_ERR("Failed to retrieve stat from message queue");
		return -1;
	}

	return ret;
}


void coap_thread(void *arg1, void *arg2, void *arg3)
{
	
	static int sock;

	#ifndef CONFIG_IP_PROTO_IPV6
	static struct sockaddr_in server = { 0 };
	#else // CONFIG_IP_PROTO_IPV6
	static struct sockaddr_in6 server = { 0 };
	#endif // CONFIG_IP_PROTO_IPV6

	static struct coap_client coap_client = { 0 };

	//initialize server

	int err;

	err = server_resolve(&server);
	if (err) {
		LOG_ERR("Failed to resolve server name");
		k_sleep(K_FOREVER);
	}
	
	#ifndef CONFIG_IP_PROTO_IPV6
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	#else // CONFIG_IP_PROTO_IPV6
	sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	#endif // CONFIG_IP_PROTO_IPV6

	if (sock < 0) {
		LOG_ERR("Failed to create CoAP socket: %d.", -errno);
		k_sleep(K_FOREVER);
	}

	LOG_INF("Initializing CoAP client");

	err = coap_client_init(&coap_client, NULL);
	if (err) {
		LOG_ERR("Failed to initialize CoAP client: %d", err);
		k_sleep(K_FOREVER);
	}

	k_sem_give(&init_sem);


	while (1) {
		//wait for semaphore
		k_sem_take(&send_sem, K_FOREVER);

		struct request_user_data *item;
        if (k_msgq_get(&coap_req_msgq, &item, K_NO_WAIT) != 0) {
            LOG_ERR("Failed to retrieve request from message queue");
			k_sem_give(&sent_sem);
            continue;
        }

		struct coap_client_request* req = ((struct request_user_data*)item)->req;
		struct coap_transmission_parameters* req_params = ((struct request_user_data*)item)->req_params;

		err = coap_client_req(&coap_client, sock, (struct sockaddr *)&server, req, req_params);
		if (err) {
			LOG_ERR("Failed to send request: %d", err);
			free_coap_request(req->user_data);
		}
		else
		{
			if(req->len==0){
				LOG_INF("CoAP request sent to %s, resource: %s",CONFIG_COAP_TEST_SERVER_HOSTNAME, req->path);
			}else if(req->len>50){
				char payload[51];
				strncpy(payload,req->payload,50);
				payload[50]='\0';
				LOG_INF("CoAP request sent to %s, resource: %s, payload: %s...",CONFIG_COAP_TEST_SERVER_HOSTNAME, req->path,payload);
			}
			else{
				LOG_INF("CoAP request sent to %s, resource: %s, payload: %s",CONFIG_COAP_TEST_SERVER_HOSTNAME, req->path,req->payload);
			}
		}
		if (k_msgq_put(&coap_ret_msgq, &err, K_NO_WAIT) != 0) {
			LOG_ERR("Failed to enqueue CoAP return value");
		}

		k_sem_give(&sent_sem);
	}
	
}

// Function to initialize the test
int coap_init() {
	
	//start thread to handle CoAP 
    k_tid_t thread_id = k_thread_create(&coap_thread_data, thread_stack,
                                        K_THREAD_STACK_SIZEOF(thread_stack),
                                        coap_thread,
                                        NULL, NULL, NULL,
                                        PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(thread_id, "coap_thread");
    k_thread_start(thread_id);

	k_sem_take(&init_sem, K_FOREVER);

	return 0;
}


