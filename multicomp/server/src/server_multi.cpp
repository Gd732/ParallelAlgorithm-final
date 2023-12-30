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

using namespace std;

int main()
{
	// init the server
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "WinSock startup error", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	vector<DTYPE> arr_full(DATANUM);
	vector_init(arr_full, DATANUM);
	vector<DTYPE> arr_recv0; vector<DTYPE> arr_recv1; vector<DTYPE> arr_recv2;


	{
#pragma omp parallel num_threads(8)
		{
#pragma omp sections
			{
#pragma omp section
				{
					SOCKET Connection0 = CreateConnection(SOCKET_PORT0);
					SendArrayToClient_multicomp(Connection0, arr_full, 0);
					vector<DTYPE> returnedVectorRef = RecvArrayBackFromClient_multicomp(Connection0);
					arr_recv0.assign(returnedVectorRef.begin(), returnedVectorRef.end());
					returnedVectorRef.clear();
					closesocket(Connection0);
				}
#pragma omp section
				{
					SOCKET Connection1 = CreateConnection(SOCKET_PORT1);
					SendArrayToClient_multicomp(Connection1, arr_full, 1);
					vector<DTYPE> returnedVectorRef = RecvArrayBackFromClient_multicomp(Connection1);
					arr_recv1.assign(returnedVectorRef.begin(), returnedVectorRef.end());
					returnedVectorRef.clear();
					closesocket(Connection1);
				}
#pragma omp section
				{
					SOCKET Connection2 = CreateConnection(SOCKET_PORT2);
					SendArrayToClient_multicomp(Connection2, arr_full, 2);
					vector<DTYPE> returnedVectorRef = RecvArrayBackFromClient_multicomp(Connection2);
					arr_recv2.assign(returnedVectorRef.begin(), returnedVectorRef.end());
					returnedVectorRef.clear();
					closesocket(Connection2);
				}
#pragma omp section
				{
					arr_full.erase(arr_full.begin() + DATANUM / 4, arr_full.end());
					test_parallel(arr_full, DATANUM / 4);
				}
			}
		}
	}
	arr_full.insert(arr_full.end(), arr_recv0.begin(), arr_recv0.end());
	cout << arr_full.size() << endl;
	arr_recv0.clear();
	merge(arr_full, 0, DATANUM / 4 - 1, DATANUM / 2 - 1);
	cout << arr_full.size() << endl;
	arr_recv1.insert(arr_recv1.end(), arr_recv2.begin(), arr_recv2.end());
	cout << "1 " << arr_recv1.size() << endl;
	cout << "2 " << arr_recv2.size() << endl;
	arr_recv2.clear();
	merge(arr_recv1, 0, DATANUM / 4 - 1, DATANUM / 2 - 1);
	cout << arr_recv1.size() << endl;

//	{
//#pragma omp parallel
//		{
//#pragma omp sections
//			{
//#pragma omp section
//				{
//					arr_full.insert(arr_full.end(), arr_recv0.begin(), arr_recv0.end());
//					arr_recv0.clear();
//					merge(arr_full, 0, DATANUM / 4 - 1, DATANUM / 2 - 1);
//				}
//#pragma omp section
//				{
//					arr_recv1.insert(arr_recv1.end(), arr_recv2.begin(), arr_recv2.end());
//					arr_recv2.clear();
//					merge(arr_recv1, 0, DATANUM / 4 - 1, DATANUM / 2 - 1);
//				}
//			}
//		}
//	}

	arr_full.insert(arr_full.end(), arr_recv1.begin(), arr_recv1.end());
	arr_recv1.clear();
	merge(arr_full, 0, DATANUM / 2 - 1, DATANUM - 1);

	cout << "arr is " << arr_full.size() << endl;
	bool is_parallel_sorted = checkSorted(arr_full, DATANUM);
	cout << (is_parallel_sorted ? "Array is sorted correctly after bi-computer parallel sorting"
		: "Array is not sorted correctly after bi-computer parallel sorting") << endl;


	//SOCKET Connection = server_init();
	//vector<DTYPE> arr_full(DATANUM);
	//vector_init(arr_full, DATANUM);

	//LARGE_INTEGER start;
	//QueryPerformanceCounter(&start);

	//// send half of the array to client
	//SendArrayToClient_bicomp(Connection, arr_full, start);

	//// sort the remaining array
	//SortArray_bicomp(arr_full);

	//// start to receive back
	//RecvArrayBackFromClient_bicomp(Connection, arr_full, start);

	//// merge the array
	//MergeArray_Check_bicomp(arr_full);

	//// get the final time cost
	//GetFinalTimeCost(start, true);

	// cleanup
	//closesocket(Connection);
	WSACleanup(); 
	return 0;
}