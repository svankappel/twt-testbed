#ifndef COAP_UTILS_H
#define COAP_UTILS_H

#include <zephyr/kernel.h>

#include <zephyr/net/coap.h>
#include <zephyr/net/socket.h>


#define TOKEN_LEN 8

#ifndef CONFIG_IP_PROTO_IPV6
int server_resolve(struct sockaddr_in* server_ptr);
#else // CONFIG_IP_PROTO_IPV6
int server_resolve(struct sockaddr_in6* server_ptr);
#endif // CONFIG_IP_PROTO_IPV6

void random_token(uint8_t *token);

void init_pending_request_pool(uint32_t max_timeout);

int add_pending_request(uint8_t * token);

uint32_t remove_pending_request(uint8_t * token);





#endif //COAP_UTILS_H