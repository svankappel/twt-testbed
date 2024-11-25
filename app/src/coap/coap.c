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

#define TOKEN_LEN 8
static uint8_t next_token[TOKEN_LEN];
static uint8_t validate_token[TOKEN_LEN];

static struct coap_packet coap_request = { 0 };
static int send_return_code = 0;
static uint8_t coap_send_buf[COAP_MAX_MSG_LEN];
static uint8_t coap_send_payload[30];

#define MAXOBSERVERS 5
static uint8_t observers = 0;
static uint8_t observer_tokens[MAXOBSERVERS][TOKEN_LEN];


static void (*coap_response_callback)(int16_t code, void * user_data) = NULL;
static void * coap_response_callback_user_data = NULL;

void coap_register_response_callback(void (*callback)(int16_t code, void * user_data),void * callback_user_data) {
	coap_response_callback_user_data = callback_user_data;
	coap_response_callback = callback;
}



int coap_put(char *resource,uint8_t *payload, uint32_t timeout)
{
	memset(coap_send_buf, 0, sizeof(coap_send_buf));
	memset(&coap_request, 0, sizeof(coap_request));

	uint8_t token[TOKEN_LEN];
	random_token(token, TOKEN_LEN);
	memcpy(next_token, token, TOKEN_LEN);

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
	if (strlen((const char *)payload) > 29) {
		strncpy((char*)coap_send_payload, (const char *)payload, 26);
		strcpy((char*)coap_send_payload + 26, "...");
	} else {
		strncpy((char*)coap_send_payload, (const char *)payload, 29);
		coap_send_payload[29] = '\0';
	}


	k_sem_give(&send_sem);
	k_sem_take(&sent_sem, K_FOREVER);

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
	random_token(token, TOKEN_LEN);
	memcpy(next_token, token, TOKEN_LEN);

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
	if (strlen((const char *)payload) > 29) {
		strncpy((char*)coap_send_payload, (const char *)payload, 26);
		strcpy((char*)coap_send_payload + 26, "...");
	} else {
		strncpy((char*)coap_send_payload, (const char *)payload, 29);
		coap_send_payload[29] = '\0';
	}


	k_sem_give(&send_sem);
	k_sem_take(&sent_sem, K_FOREVER);

	memcpy(observer_tokens[observers], token, TOKEN_LEN);
	observers++;


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
					(uint8_t *)"observe",
					strlen("observe"));
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

		k_sem_give(&send_sem);
		k_sem_take(&sent_sem, K_FOREVER);
	}
	observers = 0;
}


int coap_validate()
{
	uint8_t token[TOKEN_LEN];
	random_token(token, TOKEN_LEN);
	memcpy(validate_token, token, TOKEN_LEN);
	memcpy(next_token, token, TOKEN_LEN);

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
				(uint8_t *)"validate",
				strlen("validate"));
	if (err < 0) {
		LOG_ERR("Failed to encode CoAP option, %d\n", err);
		return err;
	}
	coap_send_payload[0] = '\0';

	k_sem_give(&send_sem);
	k_sem_take(&sent_sem, K_FOREVER);


	if (k_sem_take(&validate_sem, K_SECONDS(10)) != 0) {
		LOG_ERR("Validation timed out");
		return -ETIMEDOUT;
	}

	return send_return_code;
}

int coap_get_stat()
{

	return 0;
}


void send_coap_thread(void *arg1, void *arg2, void *arg3)
{
	int err;

	while (1) {
		//wait for semaphore
		k_sem_take(&send_sem, K_FOREVER);


		send_return_code = sendto(sock, coap_request.data, coap_request.offset, 0, (struct sockaddr *)&server, sizeof(server));
		if (err < 0) {
			LOG_ERR("Failed to send CoAP request, %d\n", errno);
		}

		char token_str[TOKEN_LEN * 3 + 1] = {0};
		for (int i = 0; i < TOKEN_LEN; i++) {
			snprintf(&token_str[i * 3], sizeof(token_str) - i * 3, " %02x", next_token[i]);
		}
		token_str[TOKEN_LEN * 3] = '\0';

		if (coap_send_payload[0] != '\0') {
			LOG_INF("CoAP request sent to: %s, Token:%s, Payload: %s", CONFIG_COAP_TEST_SERVER_HOSTNAME, token_str, coap_send_payload);
		} else {
			LOG_INF("CoAP request sent to: %s, Token:%s", CONFIG_COAP_TEST_SERVER_HOSTNAME, token_str);
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
	uint8_t temp_buf[128];
	/* STEP 9.1 - Parse the received CoAP packet */
	int err = coap_packet_parse(&reply, buf, received, NULL, 0);
	if (err < 0) {
		LOG_ERR("Malformed response received: %d\n", err);
		return err;
	}

	/* STEP 9.2 - Confirm the token in the response matches the token sent */
	int token_len = coap_header_get_token(&reply, token);
	if ((token_len != TOKEN_LEN)) {
		LOG_ERR("Invalid token received");
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
	char token_str[TOKEN_LEN * 3 + 1] = {0};
	for (int i = 0; i < TOKEN_LEN; i++) {
		snprintf(&token_str[i * 3], sizeof(token_str) - i * 3, " %02x", token[i]);
	}
	token_str[TOKEN_LEN * 3] = '\0';
	LOG_INF("CoAP response: Code 0x%x, Token:%s, Payload: %s",
	       coap_header_get_code(&reply), token_str, (char *)temp_buf);


	if(memcmp(&validate_token, token, TOKEN_LEN) == 0){
		k_sem_give(&validate_sem);
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


