#ifndef __NRPE_H
#define __NRPE_H

void create_listener(struct addrinfo *ai);
void wait_for_connections(void);
void setup_wait_conn(void);
void conn_check_peer(int sock);
void handle_connection(int);
void init_handle_conn(void);

#endif