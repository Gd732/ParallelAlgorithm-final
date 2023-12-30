//TCP server
// #define _WINSOCK_DEPRECATED_NO_WARNINGS

// #pragma comment(lib,"ws2_32.lib") 
// #include <WinSock2.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <string.h>
#include <iostream>
#include <sys/time.h>
#include "server.h" 
#include "mergesort_v1.h"
using namespace std;

void addr_init(sockaddr_in &addr)
{
	addr.sin_family = AF_INET; //IPv4 Socket
	addr.sin_port = htons(SOCKET_PORT); // sever Port
	addr.sin_addr.s_addr = inet_addr(SOCKET_IPADDR); //server PC
}

int server_init()
{
	// WSAData wsaData;
	// WORD DllVersion = MAKEWORD(2, 1);
	// if (WSAStartup(DllVersion, &wsaData) != 0)
	// {
	// 	MessageBoxA(NULL, "WinSock startup error", "Error", MB_OK | MB_ICONERROR);
	// 	return 0;
	// }

	sockaddr_in addr;
	int addrlen = sizeof(addr);
	addr_init(addr);

	int sListen = socket(AF_INET, SOCK_STREAM, 0);//接待处 
	bind(sListen, (const sockaddr*)&addr, sizeof(addr));

	listen(sListen, SOMAXCONN);

	int newConnection; //业务员  build a new socket do new connection.the sListen is just listenning not used to exchange data
	newConnection = accept(sListen, (sockaddr*)&addr, (socklen_t *)&addrlen); //newConnection is used to exchange data with client
	return newConnection;
}

bool checkSent(int bytesSent, int& Connection)
{
	if (bytesSent < 0)
	{
		return SENT_ARRAY_FAILED;
	}
	else
	{
		return SENT_ARRAY_SUCCESS;
	}
}

void us_delay(int timeout_us)
{
	auto start = chrono::system_clock::now();
	while (true)
	{
		auto duration =
			chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start).count();
		if (duration > timeout_us)
		{
			break;
		}
	}
}

int SendArrayToClient_bicomp(int& Connection, vector<DTYPE>& arr_full, timeval start)
{

	if (Connection == 0)
	{
		std::cout << "bad connection." << std::endl;
		return -1;
	}

	const char start_send = SERVER_START_SENDING;
	cout << "Start Sending Array" << endl;
	send(Connection, &start_send, 1, 0);
	int remainBytes = SORT_DATANUM * sizeof(DTYPE);
	while (remainBytes > 0)
	{
		int BytesToSend = min(SEND_RECV_BUFFER_SIZE, remainBytes);

		const char* dataToSend = reinterpret_cast<const char*>(arr_full.data()) +
			(SORT_DATANUM * sizeof(DTYPE) - remainBytes); // 指向需要发送的数据的指针

		int bytesSent = send(Connection, dataToSend, BytesToSend, 0);

		if (checkSent(bytesSent, Connection) == SENT_ARRAY_SUCCESS)
		{
			remainBytes -= bytesSent;
			continue;
		}
		else
		{
			cout << "Sent failed" << endl;
			close(Connection);
			return -1;
		}
	}
	shutdown(Connection, SHUT_WR);

	const char end_send = SERVER_END_SENDING;
	cout << "End Sending Array" << endl;
	// send(Connection, &end_send, 1, 0);

	arr_full.erase(arr_full.begin() + SORT_DATANUM, arr_full.end());
	timeval end;
	gettimeofday(&end, 0);
	double dura = TIME_DIFF(start, end);
	cout << "Sending Time Consumed:" << dura << endl;
	cout << "Remaining Array Size is " << arr_full.size() << endl;
	cout << "All data sent successfully." << endl;
	return 0;
}

int RecvArrayBackFromClient_bicomp(int& Connection, vector<DTYPE>& arr_full, timeval start)
{
	char recvArrBuffer[SEND_RECV_BUFFER_SIZE];

	char* checkbuff = new char[1];
	*checkbuff = '\0';
	recv(Connection, checkbuff, 1, 0);
	if (*checkbuff == SERVER_START_SENDING)
	{
		cout << "Start receiving array." << endl;
	}
	delete[] checkbuff;

	timeval end;
	gettimeofday(&end, NULL);
	double dura = TIME_DIFF(start, end);
	cout << "Receiving Time Consumed:" << dura << endl;

	char* arr_recv_tmp = new char[(SORT_DATANUM + 1) * sizeof(DTYPE)];
	size_t recv_len = 0;
	size_t bytesRecv = 0;
	while (recv_len < SORT_DATANUM * sizeof(DTYPE))
	{
		memset(recvArrBuffer, 0, SEND_RECV_BUFFER_SIZE);

		int bytesRecv = recv(Connection, recvArrBuffer, SEND_RECV_BUFFER_SIZE, 0);
		if (recv_len == SORT_DATANUM * sizeof(DTYPE))
		{
			break;
		}
		else if (bytesRecv < 0)
		{
			break;
		}
		memcpy(arr_recv_tmp + recv_len, recvArrBuffer, bytesRecv);
		recv_len += bytesRecv;
	}
	arr_full.insert(arr_full.end(), reinterpret_cast<DTYPE*>(arr_recv_tmp),
		reinterpret_cast<DTYPE*>(arr_recv_tmp) + SORT_DATANUM);
	delete[] arr_recv_tmp;

	return 0;
}
int RecvArrayBackFromClient_bicomp(int& Connection, vector<DTYPE>& arr_full, timeval start, DTYPE& client_max, DTYPE& client_sum)
{
	char recvArrBuffer[SEND_RECV_BUFFER_SIZE];

	char* checkbuff = new char[1];
	*checkbuff = '\0';
	recv(Connection, checkbuff, 1, NULL);
	if (*checkbuff == SERVER_START_SENDING)
	{
		cout << "Start receiving array." << endl;
	}
	delete[] checkbuff;

	timeval end;
	gettimeofday(&end, NULL);
	double dura = TIME_DIFF(start, end);
	cout << "Receiving Time Consumed:" << dura << endl;

	char* arr_recv_tmp = new char[(SORT_DATANUM + 3) * sizeof(DTYPE)];
	size_t recv_len = 0;
	size_t bytesRecv = 0;
	while (recv_len < (SORT_DATANUM + 2) * sizeof(DTYPE))
	{
		memset(recvArrBuffer, 0, SEND_RECV_BUFFER_SIZE);

		int bytesRecv = recv(Connection, recvArrBuffer, SEND_RECV_BUFFER_SIZE, NULL);
		if (recv_len == (SORT_DATANUM + 2) * sizeof(DTYPE))
		{
			break;
		}
		else if (bytesRecv < 0)
		{
			break;
		}
		memcpy(arr_recv_tmp + recv_len, recvArrBuffer, bytesRecv);
		recv_len += bytesRecv;
	}
	arr_full.insert(arr_full.end(), reinterpret_cast<DTYPE*>(arr_recv_tmp),
		reinterpret_cast<DTYPE*>(arr_recv_tmp) + (SORT_DATANUM + 2));
	//cout << arr_full.size() << endl;
	delete[] arr_recv_tmp;
	client_max = arr_full.back();
	arr_full.pop_back();
	client_sum = arr_full.back();
	arr_full.pop_back();


	return 0;
}
void SortArray_bicomp(vector<DTYPE>& arr_full)
{
	cout << "***********************************************" << endl;
	cout << "Server: start to sort" << endl;

	// sort the array
	test_parallel(arr_full, SORT_DATANUM);
}

void MergeArray_Check_bicomp(vector<DTYPE>& arr_full)
{
	cout << "Back Array Size is " << arr_full.size() << endl;
	merge(arr_full, 0, SORT_DATANUM - 1, DATANUM - 1);
	bool is_parallel_sorted = checkSorted(arr_full, DATANUM);
	cout << (is_parallel_sorted ? "Array is sorted correctly after bi-computer parallel sorting"
		: "Array is not sorted correctly after bi-computer parallel sorting") << endl;

	cout << "***********************************************" << endl;
}

double GetFinalTimeCost(timeval start, bool print_total)
{
	timeval end;
	gettimeofday(&end, NULL);
	double dura_total = TIME_DIFF(start, end);
	cout << "Receiving Time Consumed:" << dura_total << endl;

	if (print_total == true)
	{
		cout << "Total Time Consumed:" << dura_total << endl;
	}
	return dura_total;
}