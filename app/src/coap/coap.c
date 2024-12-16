#include "coap.h"
#include "coap_utils.h"

#ifdef CONFIG_COAP_SECURE
#include "coap_security.h"
#endif //CONFIG_COAP_SECURE

#include <zephyr/net/socket.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/net/tls_credentials.h>
#include <zephyr/net/coap.h>


LOG_MODULE_REGISTER(coap, CONFIG_MY_COAP_LOG_LEVEL);

#include "test_global.h"

#define STACK_SIZE 4096
#define PRIORITY 8

static K_THREAD_STACK_DEFINE(recv_thread_stack, STACK_SIZE);
struct k_thread coap_recv_thread_data;

static K_THREAD_STACK_DEFINE(send_thread_stack, STACK_SIZE);
struct k_thread coap_send_thread_data;

static K_SEM_DEFINE(send_sem, 0, 1);
static K_SEM_DEFINE(sent_sem, 0, 1);

#ifdef CONFIG_COAP_TWT_TESTBED_SERVER
static K_SEM_DEFINE(stat_sem, 0, 1);
static K_SEM_DEFINE(stat_actuator_sem, 0, 1);
static K_SEM_DEFINE(validate_sem, 0, 1);
#endif //CONFIG_COAP_TWT_TESTBED_SERVER

static int sock;

#ifndef CONFIG_IP_PROTO_IPV6
static struct sockaddr_in server = { 0 };
#else // CONFIG_IP_PROTO_IPV6
static struct sockaddr_in6 server = { 0 };
#endif // CONFIG_IP_PROTO_IPV6

#define COAP_MAX_MSG_LEN 1280

static K_MUTEX_DEFINE(coap_mutex);

static uint8_t coap_send_token[TOKEN_LEN];

#ifdef CONFIG_COAP_TWT_TESTBED_SERVER
static uint8_t validate_token[TOKEN_LEN];
static uint8_t stat_token[TOKEN_LEN];
static uint8_t stat_actuator_token[TOKEN_LEN];
static uint8_t * stat_actuator_ptr;
static int coap_stat = 0;
#endif //CONFIG_COAP_TWT_TESTBED_SERVER

static struct coap_packet coap_request = { 0 };
static int send_return_code = 0;
static uint8_t coap_send_buf[COAP_MAX_MSG_LEN];

//used for logging
static char coap_send_payload[50];
static char coap_send_resource[30];

#define MAXOBSERVERS 5
static uint8_t observers = 0;
static uint8_t observer_tokens[MAXOBSERVERS][TOKEN_LEN];
static char observer_resources[MAXOBSERVERS][30];


static void (*coap_put_response_callback)(uint32_t time, uint8_t * payload, uint16_t payload_len) = NULL;
static void (*coap_obs_response_callback)(uint8_t * payload, uint16_t payload_len) = NULL;

void coap_register_put_response_callback(void (*callback)(uint32_t time, uint8_t * payload, uint16_t payload_len)) {
	coap_put_response_callback = callback;
}

void coap_register_obs_response_callback(void (*callback)(uint8_t * payload, uint16_t payload_len)) {
	coap_obs_response_callback = callback;
}

void coap_init_pool(uint32_t requests_timeout)
{
	//initialize pending request pool
	init_pending_request_pool(requests_timeout);
}


int coap_put(char *resource,uint8_t *payload)
{
	memset(coap_send_buf, 0, sizeof(coap_send_buf));
	memset(&coap_request, 0, sizeof(coap_request));

	uint8_t token[TOKEN_LEN];
	random_token(token);

	int err = coap_packet_init(&coap_request, coap_send_buf, sizeof(coap_send_buf),
				1, COAP_TYPE_NON_CON,
				sizeof(token), (uint8_t *)&token,
				COAP_METHOD_PUT, coap_next_id());
	if (err < 0) {
		LOG_ERR("Failed to create CoAP request, %d\n", err);
		return err;
	}

	err = coap_packet_append_option(&coap_request, COAP_OPTION_URI_PATH,
				(uint8_t *)resource,
				strlen(resource));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d\n", err);
		return err;
	}
	
	const uint8_t text_plain = COAP_CONTENT_FORMAT_TEXT_PLAIN;
	err = coap_packet_append_option(&coap_request, COAP_OPTION_CONTENT_FORMAT,
				&text_plain,
				sizeof(text_plain));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d\n", err);
		return err;
	}

	err = coap_packet_append_payload_marker(&coap_request);
	if (err < 0) {
		LOG_ERR("Failed to append payload marker, %d\n", err);
		return err;
	}

	err = coap_packet_append_payload(&coap_request, (uint8_t *)payload, strlen(payload));
	if (err < 0) {
		LOG_ERR("Failed to append payload, %d\n", err);
		return err;
	}

	//set variables to be printed in log
	memcpy(coap_send_token, token, TOKEN_LEN);
	if (strlen((const char *)payload) > 49) {
		strncpy(coap_send_payload, (const char *)payload, 46);
		strcpy(coap_send_payload + 46, "...");
	} else {
		strncpy(coap_send_payload, (const char *)payload, 49);
		coap_send_payload[49] = '\0';
	}
	strcpy(coap_send_resource, resource);

	k_mutex_lock(&coap_mutex, K_FOREVER);

	k_sem_give(&send_sem);
	k_sem_take(&sent_sem, K_FOREVER);

	if (send_return_code >= 0)
	{
		err = add_pending_request(token);
		if (err < 0) {
			LOG_ERR("Failed to add pending request, %d\n", err);
			return err;
		}
	}
	k_mutex_unlock(&coap_mutex);

	return send_return_code;
}

int coap_observe(char *resource, uint8_t *payload)
{
	if (observers >= MAXOBSERVERS)
	{
		LOG_ERR("Max observers reached");
		return -ENOMEM;
	}
	
	memset(coap_send_buf, 0, sizeof(coap_send_buf));
	memset(&coap_request, 0, sizeof(coap_request));

	uint8_t token[TOKEN_LEN];
	random_token(token);

	int err = coap_packet_init(&coap_request, coap_send_buf, sizeof(coap_send_buf),
				1, COAP_TYPE_NON_CON,
				sizeof(token), (uint8_t *)&token,
				COAP_METHOD_GET, coap_next_id());
	if (err < 0) {
		LOG_ERR("Failed to create CoAP request, %d\n", err);
		return err;
	}

	err = coap_packet_append_option(&coap_request, COAP_OPTION_URI_PATH,
				(uint8_t *)resource,
				strlen(resource));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d\n", err);
		return err;
	}
	
	uint8_t observe = 0;
	err = coap_packet_append_option(&coap_request, COAP_OPTION_OBSERVE,
				&observe, sizeof(uint8_t));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP observe option, %d\n", err);
		return err;
	}

	if(payload != NULL)
	{
		err = coap_packet_append_payload_marker(&coap_request);
		if (err < 0) {
			LOG_ERR("Failed to append payload marker, %d\n", err);
			return err;
		}

		err = coap_packet_append_payload(&coap_request, (uint8_t *)payload, strlen(payload));
		if (err < 0) {
			LOG_ERR("Failed to append payload, %d\n", err);
			return err;
		}
	}

	//set variables to be printed in log
	memcpy(coap_send_token, token, TOKEN_LEN);
	if(payload != NULL)
	{
		if (strlen((const char *)payload) > 49) {
			strncpy(coap_send_payload, (const char *)payload, 46);
			strcpy(coap_send_payload + 46, "...");
		} else {
			strncpy(coap_send_payload, (const char *)payload, 49);
			coap_send_payload[49] = '\0';
		}
	}
	else
	{
		coap_send_payload[0] = '\0';
	}
	strcpy(coap_send_resource, resource);

	k_sem_give(&send_sem);
	k_sem_take(&sent_sem, K_FOREVER);

	memcpy(observer_tokens[observers], token, TOKEN_LEN);
	strcpy(observer_resources[observers], resource);
	observers++;


	return send_return_code;
}

int coap_ack(struct coap_packet * req)
{
	memset(coap_send_buf, 0, sizeof(coap_send_buf));
	memset(&coap_request, 0, sizeof(coap_request));

	int err = coap_ack_init(&coap_request, req, coap_send_buf, sizeof(coap_send_buf), 0);
	if (err < 0) {
		LOG_ERR("Failed to create CoAP ACK, %d\n", err);
		return err;
	}

	k_sem_give(&send_sem);
	k_sem_take(&sent_sem, K_FOREVER);

	return send_return_code;
}

int coap_cancel_observers()
{
	for(int i = 0; i < observers; i++)
	{
		memset(coap_send_buf, 0, sizeof(coap_send_buf));
		memset(&coap_request, 0, sizeof(coap_request));

		int err = coap_packet_init(&coap_request, coap_send_buf, sizeof(coap_send_buf),
					1, COAP_TYPE_NON_CON,
					TOKEN_LEN, (uint8_t *)&observer_tokens[i],
					COAP_METHOD_GET, coap_next_id());
		if (err < 0) {
			LOG_ERR("Failed to create CoAP request, %d\n", err);
			return err;
		}

		err = coap_packet_append_option(&coap_request, COAP_OPTION_URI_PATH,
					(uint8_t *)observer_resources[i],
					strlen(observer_resources[i]));
		if (err < 0) {
			LOG_ERR("Failed to encode CoAP option, %d\n", err);
			return err;
		}
		
		uint8_t observe = 1;
		err = coap_packet_append_option(&coap_request, COAP_OPTION_OBSERVE,
					&observe, sizeof(uint8_t));
		if (err < 0) {
			LOG_ERR("Failed to encode CoAP observe option, %d\n", err);
			return err;
		}

		//set variables to be printed in log
		memcpy(coap_send_token, &observer_tokens[i], TOKEN_LEN);
		coap_send_payload[0] = '\0';
		strcpy(coap_send_resource, observer_resources[i]);

		k_sem_give(&send_sem);
		k_sem_take(&sent_sem, K_FOREVER);
	}
	observers = 0;
	return 0;
}

#ifdef CONFIG_COAP_TWT_TESTBED_SERVER
int coap_validate()
{
	uint8_t token[TOKEN_LEN];
	random_token(token);
	memcpy(validate_token, token, TOKEN_LEN);

	memset(coap_send_buf, 0, sizeof(coap_send_buf));
	memset(&coap_request, 0, sizeof(coap_request));

	int err = coap_packet_init(&coap_request, coap_send_buf, sizeof(coap_send_buf),
				1, COAP_TYPE_NON_CON,
				sizeof(token), (uint8_t *)&token,
				COAP_METHOD_GET, coap_next_id());
	if (err < 0) {
		LOG_ERR("Failed to create CoAP request, %d\n", err);
		return err;
	}

	err = coap_packet_append_option(&coap_request, COAP_OPTION_URI_PATH,
				(uint8_t *)TESTBED_VALIDATE_RESOURCE,
				strlen(TESTBED_VALIDATE_RESOURCE));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d\n", err);
		return err;
	}

	//set variables to be printed in log
	memcpy(coap_send_token, token, TOKEN_LEN);
	coap_send_payload[0] = '\0';
	strcpy(coap_send_resource, TESTBED_VALIDATE_RESOURCE);

	for(int i = 0; i < 3; i++)
	{
		k_sem_give(&send_sem);
		k_sem_take(&sent_sem, K_FOREVER);

		if (k_sem_take(&validate_sem, K_SECONDS(5)) != 0) {
			if(i == 2)
			{
				LOG_ERR("Validation timed out!");
				return -ETIMEDOUT;
			}
			LOG_WRN("Validation timed out, Retrying ...");
			continue;
		}
		break;
	}

	return 0;
}

int coap_get_stat()
{
	coap_stat = 0;
	uint8_t token[TOKEN_LEN];
	random_token(token);
	memcpy(stat_token, token, TOKEN_LEN);

	memset(coap_send_buf, 0, sizeof(coap_send_buf));
	memset(&coap_request, 0, sizeof(coap_request));

	int err = coap_packet_init(&coap_request, coap_send_buf, sizeof(coap_send_buf),
				1, COAP_TYPE_NON_CON,
				sizeof(token), (uint8_t *)&token,
				COAP_METHOD_GET, coap_next_id());
	if (err < 0) {
		LOG_ERR("Failed to create CoAP request, %d\n", err);
		return err;
	}

	err = coap_packet_append_option(&coap_request, COAP_OPTION_URI_PATH,
				(uint8_t *)TESTBED_STAT_RESOURCE,
				strlen(TESTBED_STAT_RESOURCE));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d\n", err);
		return err;
	}

	//set variables to be printed in log
	coap_send_payload[0] = '\0';
	strcpy(coap_send_resource, TESTBED_STAT_RESOURCE);
	memcpy(coap_send_token, token, TOKEN_LEN);

	for(int i = 0; i < 3; i++)
	{
		k_sem_give(&send_sem);
		k_sem_take(&sent_sem, K_FOREVER);

		if (k_sem_take(&stat_sem, K_SECONDS(5)) != 0) {
			if(i == 2)
			{
				LOG_ERR("Get stat timed out!");
				return -ETIMEDOUT;
			}
			LOG_WRN("Get stat timed out, Retrying ...");
			continue;
		}
		break;
	}

	return coap_stat;
}

int coap_get_actuator_stat(char * buffer)
{
	coap_stat = 0;
	uint8_t token[TOKEN_LEN];
	random_token(token);
	memcpy(stat_actuator_token, token, TOKEN_LEN);

	memset(coap_send_buf, 0, sizeof(coap_send_buf));
	memset(&coap_request, 0, sizeof(coap_request));

	int err = coap_packet_init(&coap_request, coap_send_buf, sizeof(coap_send_buf),
				1, COAP_TYPE_NON_CON,
				sizeof(token), (uint8_t *)&token,
				COAP_METHOD_GET, coap_next_id());
	if (err < 0) {
		LOG_ERR("Failed to create CoAP request, %d\n", err);
		return err;
	}

	err = coap_packet_append_option(&coap_request, COAP_OPTION_URI_PATH,
				(uint8_t *)TESTBED_ACTUATOR_STAT_RESOURCE,
				strlen(TESTBED_ACTUATOR_STAT_RESOURCE));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d\n", err);
		return err;
	}

	//set variables to be printed in log
	coap_send_payload[0] = '\0';
	strcpy(coap_send_resource, TESTBED_ACTUATOR_STAT_RESOURCE);
	memcpy(coap_send_token, token, TOKEN_LEN);

	//set pointer to buffer
	stat_actuator_ptr = (uint8_t *)buffer;

	for(int i = 0; i < 3; i++)
	{
		k_sem_give(&send_sem);
		k_sem_take(&sent_sem, K_FOREVER);

		if (k_sem_take(&stat_actuator_sem, K_SECONDS(5)) != 0) {
			if(i == 2)
			{
				LOG_ERR("Get actuator stat timed out!");
				return -ETIMEDOUT;
			}
			LOG_WRN("Get actuator stat timed out, Retrying ...");
			continue;
		}
		break;
	}
	
	return 0;
}

#endif // CONFIG_COAP_TWT_TESTBED_SERVER


void send_coap_thread(void *arg1, void *arg2, void *arg3)
{
	while (1) {
		//wait for semaphore
		k_sem_take(&send_sem, K_FOREVER);


		send_return_code = sendto(sock, coap_request.data, coap_request.offset, 0, (struct sockaddr *)&server, sizeof(server));
		if (send_return_code < 0) {
			LOG_ERR("Failed to send CoAP request, %d\n", errno);
		}
		//log
		if(coap_request.data[1] == 0)	//if message is an ack
		{
			LOG_INF("CoAP ACK sent to: %s", CONFIG_COAP_TEST_SERVER_HOSTNAME);
		}
		else
		{
			char token_str[TOKEN_LEN * 3 + 1] = {0};
			for (int i = 0; i < TOKEN_LEN; i++) {
				snprintf(&token_str[i * 3], sizeof(token_str) - i * 3, " %02x", coap_send_token[i]);
			}
			token_str[TOKEN_LEN * 3] = '\0';

			if (coap_send_payload[0] != '\0') {
				LOG_INF("CoAP request sent to: %s, Token:%s, Resource: %s, Payload: %s", CONFIG_COAP_TEST_SERVER_HOSTNAME, token_str, coap_send_resource, coap_send_payload);
			} else {
				LOG_INF("CoAP request sent to: %s, Token:%s, Resource: %s", CONFIG_COAP_TEST_SERVER_HOSTNAME, token_str, coap_send_resource);
			}
		}

		

		k_sem_give(&sent_sem);
	}
}

static int client_handle_response(uint8_t *buf, int received)
{
	struct coap_packet reply;
	uint8_t token[TOKEN_LEN];
	const uint8_t *payload;
	uint16_t payload_len;
	uint8_t temp_buf[50];

	// Parse the received CoAP packet
	int err = coap_packet_parse(&reply, buf, received, NULL, 0);
	if (err < 0) {
		LOG_ERR("Malformed response received: %d\n", err);
		return err;
	}

	//Retrieve the token
	int token_len = coap_header_get_token(&reply, token);
	if ((token_len != TOKEN_LEN)) {
		LOG_ERR("Invalid token received");
		return 0;
	}

	//Retrieve the payload and confirm it's nonzero
	payload = coap_packet_get_payload(&reply, &payload_len);

	if (payload_len > 0) {
		if (payload_len > 49) {
			strncpy((char*)temp_buf, (const char *)payload, 46);
			strcpy((char*)temp_buf + 46, "...");
		} else {
			strncpy((char*)temp_buf, (const char *)payload, payload_len);
			temp_buf[payload_len] = '\0';
		}
	} else {
		temp_buf[0] = '\0';
	}


	//Log the header code, token and payload of the response
	char token_str[TOKEN_LEN * 3 + 1] = {0};
	for (int i = 0; i < TOKEN_LEN; i++) {
		snprintf(&token_str[i * 3], sizeof(token_str) - i * 3, " %02x", token[i]);
	}
	token_str[TOKEN_LEN * 3] = '\0';

	if (payload_len > 0) {
		LOG_INF("CoAP response: Code 0x%x, Token:%s, Payload: %s",
	       coap_header_get_code(&reply), token_str, (char *)temp_buf);
	} else {
		LOG_INF("CoAP response: Code 0x%x, Token:%s",
	       coap_header_get_code(&reply), token_str);
	}

	#ifdef CONFIG_COAP_TWT_TESTBED_SERVER

	//Check if the response is a validation response
	if(memcmp(&validate_token, token, TOKEN_LEN) == 0 &&
		coap_header_get_code(&reply) == COAP_RESPONSE_CODE_CONTENT &&
		strncmp((const char *)payload, "valid", 5) == 0) 
	{
		k_sem_give(&validate_sem);
		return 0;
	}

	//check if the response is a stat response
	if (memcmp(&stat_token, token, TOKEN_LEN) == 0 &&
		coap_header_get_code(&reply) == COAP_RESPONSE_CODE_CONTENT)
	{
		char temp_payload[payload_len + 1];
		strncpy(temp_payload, (const char *)payload, payload_len);
		coap_stat = atoi(temp_payload);
		k_sem_give(&stat_sem);
		return 0;
	}

	//check if the response is an actuator stat response
	if (memcmp(&stat_actuator_token, token, TOKEN_LEN) == 0 &&
		coap_header_get_code(&reply) == COAP_RESPONSE_CODE_CONTENT)
	{
		strncpy((char *)stat_actuator_ptr, (const char *)payload, payload_len);
		k_sem_give(&stat_actuator_sem);
		return 0;
	}
	#endif // CONFIG_COAP_TWT_TESTBED_SERVER

	//check if the response is an observer response
	for(int i = 0; i < observers; i++)
	{
		if(memcmp(observer_tokens[i], token, TOKEN_LEN) == 0 &&
			coap_header_get_code(&reply) == COAP_RESPONSE_CODE_CONTENT)
		{
			if((reply.data[0] & 0b00110000) == 0)	//if confirmalble -> ack
			{
				coap_ack(&reply);
			}
			
			if (coap_obs_response_callback != NULL) {
				coap_obs_response_callback((uint8_t *)payload, payload_len);
			}
			return 0;
		}
	}

	//check if the response is a put response
	if(coap_header_get_code(&reply) == COAP_RESPONSE_CODE_CHANGED)
	{
		uint32_t time = remove_pending_request(token);
		if(time > 0)
		{
			if (coap_put_response_callback != NULL) {
				coap_put_response_callback(time, (uint8_t *)payload, payload_len);
			}
			return 0;
		}
	}
	
	
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
		k_mutex_lock(&coap_mutex, K_FOREVER);
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
		k_mutex_unlock(&coap_mutex);
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

	#ifndef CONFIG_COAP_SECURE
	
	#ifndef CONFIG_IP_PROTO_IPV6
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	#else // CONFIG_IP_PROTO_IPV6
	sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	#endif // CONFIG_IP_PROTO_IPV6

	#else // CONFIG_COAP_SECURE

	#ifndef CONFIG_IP_PROTO_IPV6
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_DTLS_1_2);
	err = set_socket_dtls_options(sock);
	#else // CONFIG_IP_PROTO_IPV6
	sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_DTLS_1_2);
	err = set_socket_dtls_options(sock);
	#endif // CONFIG_IP_PROTO_IPV6

	if (err) {
		LOG_ERR("Failed to set socket options, %d", err);
		k_sleep(K_FOREVER);
	}

	#endif // CONFIG_COAP_SECURE

	if (sock < 0) {
		LOG_ERR("Failed to create CoAP socket: %d.", -errno);
		k_sleep(K_FOREVER);
	}


	
	//start threads to handle CoAP send and receive
    k_thread_create(&coap_send_thread_data, send_thread_stack,
                                        K_THREAD_STACK_SIZEOF(send_thread_stack),
                                        send_coap_thread,
                                        NULL, NULL, NULL,
                                        PRIORITY, 0, K_NO_WAIT);
	k_thread_create(&coap_recv_thread_data, recv_thread_stack,
                                        K_THREAD_STACK_SIZEOF(recv_thread_stack),
                                        recv_coap_thread,
                                        NULL, NULL, NULL,
                                        PRIORITY, 0, K_NO_WAIT);
    


	return 0;
}


