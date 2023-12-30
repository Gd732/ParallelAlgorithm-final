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

#define DTYPE float

void addr_init(SOCKADDR_IN& addr);
SOCKET client_init();
bool checkSent(int bytesSent, SOCKET& Connection);
std::vector<DTYPE> RecvArrayFromServer_bicomp(SOCKET& Connection, LARGE_INTEGER start);
int SendArrayBackToServer_bicomp(SOCKET& Connection, std::vector<DTYPE>& arr_recv);
int SendArrayBackToServer_bicomp(SOCKET& Connection, std::vector<DTYPE>& arr_recv, DTYPE client_max, DTYPE client_sum);

void CheckRemainArray_bicomp(std::vector<DTYPE>& arr_recv);

