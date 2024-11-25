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
#include <zephyr/random/rand32.h>

LOG_MODULE_REGISTER(coap, CONFIG_MY_COAP_LOG_LEVEL);


#define STACK_SIZE 4096
#define PRIORITY 8

static K_THREAD_STACK_DEFINE(recv_thread_stack, STACK_SIZE);
struct k_thread coap_recv_thread_data;

static K_THREAD_STACK_DEFINE(send_thread_stack, STACK_SIZE);
struct k_thread coap_send_thread_data;

static K_SEM_DEFINE(send_sem, 0, 1);
static K_SEM_DEFINE(sent_sem, 0, 1);

static K_SEM_DEFINE(validate_sem, 0, 1);

static int sock;

#ifndef CONFIG_IP_PROTO_IPV6
static struct sockaddr_in server = { 0 };
#else // CONFIG_IP_PROTO_IPV6
static struct sockaddr_in6 server = { 0 };
#endif // CONFIG_IP_PROTO_IPV6

// Define the macros for the CoAP version and message length
#define COAP_MAX_MSG_LEN 1280

uint16_t next_token;

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
	if(coap_response_callback)
	{
		(*coap_response_callback)(code, coap_response_callback_user_data);
	}
}

static void observe_response_cb(int16_t code, size_t offset, const uint8_t *payload,
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

	struct coap_client_request* req = alloc_coap_request(resource_len, strlen(payload)+1,false);
	struct coap_transmission_parameters* req_params = ((struct request_user_data*)req->user_data)->req_params;

	
	req->method = COAP_METHOD_PUT;
	req->confirmable = false;
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
		free_coap_request(req->user_data);
		return -ENOEXEC;
	}

	
	free_coap_request(req->user_data);
	return ret;
}

int coap_observe(char *resource,uint8_t *payload, bool start_observe)
{

	size_t resource_len = strlen(resource)+1;

	struct coap_client_request* req = alloc_coap_request(resource_len, strlen(payload)+1,true);
	struct coap_transmission_parameters* req_params = ((struct request_user_data*)req->user_data)->req_params;

	req->method = COAP_METHOD_GET;
	req->confirmable = false;
	req->fmt = COAP_CONTENT_FORMAT_TEXT_PLAIN;
	strcpy(req->payload,payload);
	req->len = strlen(payload);
	req->cb = observe_response_cb;
	strncpy((char*)req->path, resource, resource_len);
	req->options->code = COAP_OPTION_OBSERVE;
	req->options->len = 1;
	req->options->value[0] = start_observe ? 0 : 1 ;
	req->num_options = 1;

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
		free_coap_request(req->user_data);
		return -ENOEXEC;
	}
	free_coap_request(req->user_data);
	return ret;
}


int coap_validate()
{
	char *resource_path = "validate";
	size_t resource_path_len = strlen(resource_path)+1;

	struct coap_client_request* req = alloc_coap_request(resource_path_len, 0,false);
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
		free_coap_request(req->user_data);
		return -ENOEXEC;
	}

	if (k_sem_take(&validate_sem, K_SECONDS(10)) != 0) {
		LOG_ERR("Validation timed out");
		free_coap_request(req->user_data);
		return -ETIMEDOUT;
	}
	free_coap_request(req->user_data);
	return ret;
}

int coap_get_stat()
{
	char *resource_path = "stat";
	size_t resource_path_len = strlen(resource_path)+1;

	struct coap_client_request* req = alloc_coap_request(resource_path_len, 0,false);
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
		free_coap_request(req->user_data);
        return -ENOMEM;
    }

	k_sem_give(&send_sem);
	k_sem_take(&sent_sem, K_FOREVER);

	int ret;

	if (k_msgq_get(&coap_ret_msgq, &ret, K_NO_WAIT) != 0) {
		LOG_ERR("Failed to retrieve return value from message queue");
		free_coap_request(req->user_data);
		return -ENOEXEC;
	}

	if (k_msgq_get(&coap_stat_msgq, &ret, K_SECONDS(10)) != 0) {
		LOG_ERR("Failed to retrieve stat from message queue");
		free_coap_request(req->user_data);
		return -1;
	}

	free_coap_request(req->user_data);

	return ret;
}


void send_coap_thread(void *arg1, void *arg2, void *arg3)
{
	int err;

	uint8_t coap_buf[COAP_MAX_MSG_LEN];

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

		struct coap_packet request;
		uint16_t token = sys_rand32_get();
		next_token = token;
		err = coap_packet_init(&request, coap_buf, sizeof(coap_buf),
			       1, COAP_TYPE_NON_CON,
			       sizeof(token), (uint8_t *)&token,
			       COAP_METHOD_PUT, coap_next_id());
		if (err < 0) {
			LOG_ERR("Failed to create CoAP request, %d\n", err);
			return err;
		}

		err = coap_packet_append_option(&request, COAP_OPTION_URI_PATH,
					(uint8_t *)"sensor",
					strlen("sensor"));
		if (err < 0) {
			LOG_ERR("Failed to encode CoAP option, %d\n", err);
			return err;
		}

		const uint8_t text_plain = COAP_CONTENT_FORMAT_TEXT_PLAIN;
		err = coap_packet_append_option(&request, COAP_OPTION_CONTENT_FORMAT,
					&text_plain,
					sizeof(text_plain));
		if (err < 0) {
			LOG_ERR("Failed to encode CoAP option, %d\n", err);
			return err;
		}

		err = coap_packet_append_payload_marker(&request);
		if (err < 0) {
			LOG_ERR("Failed to append payload marker, %d\n", err);
			return err;
		}

		err = coap_packet_append_payload(&request, (uint8_t *)"test", sizeof("test"));
		if (err < 0) {
			LOG_ERR("Failed to append payload, %d\n", err);
			return err;
		}

		err = sendto(sock, request.data, request.offset, 0, (struct sockaddr *)&server, sizeof(server));
		if (err < 0) {
			LOG_ERR("Failed to send CoAP request, %d\n", errno);
			return -errno;
		}


		if (k_msgq_put(&coap_ret_msgq, &err, K_NO_WAIT) != 0) {
			LOG_ERR("Failed to enqueue CoAP return value");
		}

		k_sem_give(&sent_sem);

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
	/* STEP 9.1 - Parse the received CoAP packet */
	int err = coap_packet_parse(&reply, buf, received, NULL, 0);
	if (err < 0) {
		LOG_ERR("Malformed response received: %d\n", err);
		return err;
	}

	/* STEP 9.2 - Confirm the token in the response matches the token sent */
	token_len = coap_header_get_token(&reply, token);
	if ((token_len != sizeof(uint16_t)) ||
	    (memcmp(&next_token, token, sizeof(next_token)) != 0)) {
		LOG_ERR("Invalid token received: 0x%02x%02x\n",
		       token[1], token[0]);
		return 0;
	}

	/* STEP 9.3 - Retrieve the payload and confirm it's nonzero */
	payload = coap_packet_get_payload(&reply, &payload_len);

	if (payload_len > 0) {
		snprintf(temp_buf, MIN(payload_len + 1, sizeof(temp_buf)), "%s", payload);
	} else {
		strcpy(temp_buf, "EMPTY");
	}

	/* STEP 9.4 - Log the header code, token and payload of the response */
	LOG_INF("CoAP response: Code 0x%x, Token 0x%02x%02x, Payload: %s\n",
	       coap_header_get_code(&reply), token[1], token[0], (char *)temp_buf);

	return 0;
}


void recv_coap_thread(void *arg1, void *arg2, void *arg3)
{
	struct pollfd fds;
    fds.fd = sock;
    fds.events = POLLIN;
	

	uint8_t coap_buf[COAP_MAX_MSG_LEN];

    while (1) {
        int ret = poll(&fds, 1, -1); // Use -1 for infinite timeout
        if (ret > 0) {
            if (fds.revents & POLLIN) {
				socklen_t server_len = sizeof(server);
                int len = recvfrom(sock, coap_buf, sizeof(coap_buf), 0, (struct sockaddr *)&server, &server_len);
                int err = client_handle_response(coap_buf, len);
				if (err < 0) {
					LOG_ERR("Invalid response, exit\n");
					break;
				}
            }
        } else if (ret < 0) {
            LOG_ERR("Poll error");
        }
    }
}

// Function to initialize the test
int coap_init() {

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


	
	//start thread to handle CoAP 
    k_tid_t send_thread_id = k_thread_create(&coap_send_thread_data, send_thread_stack,
                                        K_THREAD_STACK_SIZEOF(send_thread_stack),
                                        send_coap_thread,
                                        NULL, NULL, NULL,
                                        PRIORITY, 0, K_NO_WAIT);
	k_tid_t recv_thread_id = k_thread_create(&coap_recv_thread_data, recv_thread_stack,
                                        K_THREAD_STACK_SIZEOF(recv_thread_stack),
                                        recv_coap_thread,
                                        NULL, NULL, NULL,
                                        PRIORITY, 0, K_NO_WAIT);
    


	return 0;
}


