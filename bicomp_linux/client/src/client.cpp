//TCP client
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// #pragma comment(lib,"ws2_32.lib")
// #include <WinSock2.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
// #include <windows.h>
#include <vector>
#include <sys/time.h>
#include "client.h"
#include "mergesort_v1.h"

using namespace std;

int main()
{
	int Connection = client_init();
	

	// LARGE_INTEGER start;
	// QueryPerformanceCounter(&start);

	timeval start;
	gettimeofday(&start, NULL);

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
	close(Connection);
	return 0;
}

