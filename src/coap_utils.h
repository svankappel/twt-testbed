#ifndef COAP_UTILS_H
#define COAP_UTILS_H

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>


#ifndef CONFIG_IP_PROTO_IPV6
int server_resolve(struct sockaddr_in* server_ptr);
#else
int server_resolve(struct sockaddr_in6* server_ptr);
#endif

struct coap_client_request *alloc_coap_request(uint16_t path_len, uint16_t payload_len);

void free_coap_request(void * data);

struct request_user_data {
	struct coap_client_request *req;
	struct coap_transmission_parameters *req_params;
};

#endif