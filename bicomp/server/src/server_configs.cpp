//TCP server
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib") 
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include "server.h" 
#include "mergesort_v1.h"
using namespace std;

void addr_init(SOCKADDR_IN &addr)
{
	addr.sin_family = AF_INET; //IPv4 Socket
	addr.sin_port = htons(SOCKET_PORT); // sever Port
	addr.sin_addr.s_addr = inet_addr(SOCKET_IPADDR); //server PC
}

SOCKET server_init()
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "WinSock startup error", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	SOCKADDR_IN addr;
	int addrlen = sizeof(addr);
	addr_init(addr);

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);//接待处 
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));

	listen(sListen, SOMAXCONN);

	SOCKET newConnection; //业务员  build a new socket do new connection.the sListen is just listenning not used to exchange data
	newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //newConnection is used to exchange data with client
	return newConnection;
}

bool checkSent(int bytesSent, SOCKET& Connection)
{
	if (bytesSent == SOCKET_ERROR)
	{
		cerr << "Error sending data: " << WSAGetLastError() << endl;
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

int SendArrayToClient_bicomp(SOCKET& Connection, vector<DTYPE>& arr_full, LARGE_INTEGER start)
{

	if (Connection == 0)
	{
		std::cout << "bad connection." << std::endl;
		return -1;
	}

	const char start_send = SERVER_START_SENDING;
	cout << "Start Sending Array" << endl;
	send(Connection, &start_send, 1, NULL);
	int remainBytes = SORT_DATANUM * sizeof(DTYPE);
	while (remainBytes > 0)
	{
		int BytesToSend = min(SEND_RECV_BUFFER_SIZE, remainBytes);

		const char* dataToSend =
			reinterpret_cast<const char*>(arr_full.data()) + SORT_DATANUM * sizeof(DTYPE) + //初始指针位置
			(SORT_DATANUM * sizeof(DTYPE) - remainBytes); //随发送而移动
			

		int bytesSent = send(Connection, dataToSend, BytesToSend, 0);

		if (checkSent(bytesSent, Connection) == SENT_ARRAY_SUCCESS)
		{
			remainBytes -= bytesSent;
			continue;
		}
		else
		{
			cout << "Sent failed" << endl;
			closesocket(Connection);
			WSACleanup();
			return -1;
		}
	}
	shutdown(Connection, SD_SEND);

	const char end_send = SERVER_END_SENDING;
	cout << "End Sending Array" << endl;
	send(Connection, &end_send, 1, NULL);

	arr_full.erase(arr_full.begin() + SORT_DATANUM, arr_full.end());
	LARGE_INTEGER mark_server_send;
	QueryPerformanceCounter(&mark_server_send);
	size_t time_server_send = mark_server_send.QuadPart - start.QuadPart;
	cout << "Sending Time Consumed:" << time_server_send << endl;
	cout << "Remaining Array Size is " << arr_full.size() << endl;
	cout << "All data sent successfully." << endl;
	return 0;
}

int RecvArrayBackFromClient_bicomp(SOCKET& Connection, vector<DTYPE>& arr_full, LARGE_INTEGER start)
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

	LARGE_INTEGER mark_server_recv_back;
	QueryPerformanceCounter(&mark_server_recv_back);
	size_t time_server_recv = mark_server_recv_back.QuadPart - start.QuadPart;

	char* arr_recv_tmp = new char[(SORT_DATANUM + 1) * sizeof(DTYPE)];
	size_t recv_len = 0;
	size_t bytesRecv = 0;
	while (recv_len < SORT_DATANUM * sizeof(DTYPE))
	{
		memset(recvArrBuffer, 0, SEND_RECV_BUFFER_SIZE);

		int bytesRecv = recv(Connection, recvArrBuffer, SEND_RECV_BUFFER_SIZE, NULL);
		if (recv_len == SORT_DATANUM * sizeof(DTYPE))
		{
			break;
		}
		else if (bytesRecv < 0)
		{
			std::cerr << "Error receiving data: " << WSAGetLastError() << std::endl;
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

size_t GetFinalTimeCost(LARGE_INTEGER start, bool print_total)
{
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	size_t time_total = end.QuadPart - start.QuadPart;
	if (print_total == true)
	{
		cout << "Total Time Consumed:" << time_total << endl;
	}
	return time_total;
}