//TCP client
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <vector>
#include <time.h>
#include "client.h"
#include "mergesort_v1.h"

using namespace std;

int main()
{
	SOCKET Connection = client_init();
	

	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);

	// receive array from server
	vector<DTYPE> arr_recv = RecvArrayFromServer_bicomp(Connection, start);
	
	// sorting array
	cout << "Client: start to sort" << endl;
	test_parallel(arr_recv, SORT_DATANUM); 

	// send array back
	SendArrayBackToServer_bicomp(Connection, arr_recv);

	// check remain array
	CheckRemainArray_bicomp(arr_recv);

	//cleanup
	closesocket(Connection);
	WSACleanup();
	return 0;
}

