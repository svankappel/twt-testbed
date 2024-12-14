#ifndef COAP_SECURITY_H
#define COAP_SECURITY_H

#ifdef CONFIG_COAP_SECURE

int configure_psk(void);


int set_socket_dtls_options(int sock);

#endif //CONFIG_COAP_SECURE

#endif //COAP_SECURITY_H