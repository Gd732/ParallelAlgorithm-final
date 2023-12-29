#pragma once
#define SOCKET_IPADDR "100.80.168.19"
#define SOCKET_PORT 8888
#define SEND_RECV_BUFFER_SIZE 1024
#define SERVER_START_SENDING '^' 
#define SERVER_END_SENDING '_'
#define CLIENT_START_SENDING '^' 
#define CLIENT_END_SENDING '_'

#define SENT_ARRAY_SUCCESS 0x00
#define SENT_ARRAY_FAILED  0x01


void addr_init(SOCKADDR_IN& addr);
SOCKET server_init(); 
bool checkSent(int bytesSent, SOCKET& Connection);
void us_delay(int timeout_us);
