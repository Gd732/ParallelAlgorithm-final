#pragma once
// #define SOCKET_IPADDR "100.80.168.19"
#define SOCKET_IPADDR "192.168.222.133"

#define SOCKET_PORT 8888
#define SEND_RECV_BUFFER_SIZE 1024
#define SERVER_START_SENDING '^' 
#define SERVER_END_SENDING '_'
#define CLIENT_START_SENDING '^' 
#define CLIENT_END_SENDING '_'

#define SENT_ARRAY_SUCCESS 0x00
#define SENT_ARRAY_FAILED  0x01

#define DTYPE float

void addr_init(sockaddr_in &addr);
int server_init();
bool checkSent(int bytesSent, int& Connection);
void us_delay(int timeout_us);
int SendArrayToClient_bicomp(int& Connection, std::vector<DTYPE>& arr_full, timeval start);
int RecvArrayBackFromClient_bicomp(int& Connection, std::vector<DTYPE>& arr_full, timeval start);
int RecvArrayBackFromClient_bicomp(int& Connection, std::vector<DTYPE>& arr_full, timeval start, DTYPE& client_max, DTYPE& client_sum);

void SortArray_bicomp(std::vector<DTYPE>& arr_full);
void MergeArray_Check_bicomp(std::vector<DTYPE>& arr_full);
double GetFinalTimeCost(timeval start, bool print_total);


