#pragma once

#define SOCKET_IPADDR "100.80.168.19"
#define SOCKET_PORT 8888
#define SEND_RECV_BUFFER_SIZE 1024
#define SERVER_START_SENDING '^'


void addr_init(SOCKADDR_IN& addr);
SOCKET client_init();
