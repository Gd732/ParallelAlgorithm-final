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
#include "max_sum.h"
#include "server.h"
#include "mergesort_v1.h"

using namespace std;

int main()
{
	// init the server
	SOCKET Connection = server_init();
	vector<DTYPE> arr_full(DATANUM);
	vector_init(arr_full, DATANUM);

	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);

	// send half of the array to client
	SendArrayToClient_bicomp(Connection, arr_full, start);

	// sort the remaining array
	SortArray_bicomp(arr_full);
	DTYPE server_max = arrayMaxParallel(arr_full, SORT_DATANUM);
	DTYPE server_sum = arraySumParallel(arr_full, SORT_DATANUM);

	// start to receive back
	RecvArrayBackFromClient_bicomp(Connection, arr_full, start);

	// merge the array
	MergeArray_Check_bicomp(arr_full);

	// get the final time cost
	GetFinalTimeCost(start, true);

	// cleanup
	closesocket(Connection);
	WSACleanup(); 
	return 0;
}