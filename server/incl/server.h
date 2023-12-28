#pragma once
#define SOCKET_IPADDR "100.80.168.19"
#define SOCKET_PORT 8888
#define SEND_RECV_BUFFER_SIZE 1024
#define SERVER_START_SENDING '^'
#define SERVER_SENT_SUCCESS 0x00
#define SERVER_SENT_FAILED  0x01


void addr_init(SOCKADDR_IN& addr);
SOCKET server_init();
bool checkSent(int bytesSent, SOCKET& Connection);
