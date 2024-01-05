//TCP server
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <iostream>
#include <Windows.h>
#include <vector>
#include <chrono>
#include "server.h"
#include "mergesort_v1.h"
#include "max_sum.h"

using namespace std;

int main()
{
	
	// init the server
	SOCKET Connection = server_init();
	vector<DTYPE> arr_full(DATANUM);
	vector_init(arr_full, DATANUM);

	DTYPE client_max = 0, client_sum = 0;

	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);

	// send half of the array to client
	LARGE_INTEGER start_send_to_client, end_send_to_client;
	QueryPerformanceCounter(&start_send_to_client);
	SendArrayToClient_bicomp(Connection, arr_full, start);
	QueryPerformanceCounter(&end_send_to_client);
	size_t time_send_to_client = end_send_to_client.QuadPart - start_send_to_client.QuadPart;
	cout << "Sending Time Consumed:" << time_send_to_client << endl;

	// sort the remaining array
	SortArray_bicomp(arr_full);
	cout << "***********************************************" << endl;

	LARGE_INTEGER start_max, end_max;
	QueryPerformanceCounter(&start_max);
	DTYPE server_max = arrayMaxParallel(arr_full, SORT_DATANUM);
	QueryPerformanceCounter(&end_max);
	size_t time_max = end_max.QuadPart - start_max.QuadPart;
	cout << "server max: " << server_max << endl;
	cout << "Get Max Time Consumed:" << time_max << endl;
	cout << "***********************************************" << endl;

	LARGE_INTEGER start_sum, end_sum;
	QueryPerformanceCounter(&start_sum);
	DTYPE server_sum = arraySumKahanParallel(arr_full, SORT_DATANUM);
	QueryPerformanceCounter(&end_sum);
	size_t time_sum = end_sum.QuadPart - start_sum.QuadPart;
	cout << "server sum: " << server_sum << endl;
	cout << "Get Sum Time Consumed:" << time_sum << endl;
	cout << "***********************************************" << endl;

	// start to receive back
	LARGE_INTEGER start_recv, end_recv;
	QueryPerformanceCounter(&start_recv);
	RecvArrayBackFromClient_bicomp(Connection, arr_full, start, client_max, client_sum);
	QueryPerformanceCounter(&end_recv);
	size_t time_recv = end_recv.QuadPart - start_recv.QuadPart;
	cout << "Recv Time Consumed:" << time_recv << endl;
	cout << "client max: " << client_max << endl;
	cout << "client_sum: " << client_sum << endl;
	cout << "***********************************************" << endl;

	DTYPE max = (client_max > server_max) ? client_max : server_max;
	DTYPE sum = client_sum + server_sum;
	cout << "max = " << max << endl;
	cout << "sum = " << sum << endl;

	// merge the array
	MergeArray_Check_bicomp(arr_full);

	// get the final time cost
	GetFinalTimeCost(start, true);

	// cleanup
	closesocket(Connection);
	WSACleanup(); 
	return 0;
}